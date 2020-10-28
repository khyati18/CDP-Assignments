#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include<fcntl.h> 

#include "tokenizer.h"

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

int cmd_exit(struct tokens *tokens);
int cmd_help(struct tokens *tokens);
int cmd_id(struct tokens *tokens);

/* Built-in command functions take token array and return int */
typedef int cmd_fun_t(struct tokens *tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_exit, "exit", "exit the command shell"},
  {cmd_id, "id", "displays the user-id, the primary group-id and the groups the user is part of"},
};

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens *tokens) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
  return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens *tokens) {
  exit(0);
}

/* id command */
int cmd_id(unused struct tokens *tokens){
	printf("uid is %d\n", getuid());
	printf("gid is %d\n", getgid());	

	__uid_t uid = getuid();

	struct passwd* pw = getpwuid(uid);
	if(pw == NULL){
    	perror("getpwuid error: ");
	}
	int ngroups = 0;

	//this call is just to get the correct ngroups
	getgrouplist(pw->pw_name, pw->pw_gid, NULL, &ngroups);
	__gid_t groups[ngroups];

	//here we actually get the groups
	getgrouplist(pw->pw_name, pw->pw_gid, groups, &ngroups);

	printf("Groups the user is part of -\n");
	//example to print the groups name
	for (int i = 0; i < ngroups; i++){
	    struct group* gr = getgrgid(groups[i]);
	    if(gr == NULL){
	        perror("getgrgid error: ");
	    }
	    printf("%d, (%s)\n",gr->gr_gid,gr->gr_name);
	}

	return 1;
}


/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
      return i;
  return -1;
}

/* Intialization procedures for this shell */
void init_shell() {
  /* Our shell is connected to standard input. */
  shell_terminal = STDIN_FILENO;

  /* Check if we are running interactively */
  shell_is_interactive = isatty(shell_terminal);

  if (shell_is_interactive) {
    /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
     * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
     * foreground, we'll receive a SIGCONT. */
    while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
      kill(-shell_pgid, SIGTTIN);

    /* Saves the shell's process id */
    shell_pgid = getpid();

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Save the current termios to a variable, so it can be restored later. */
    tcgetattr(shell_terminal, &shell_tmodes);
  }
}

int main(unused int argc, unused char *argv[]) {
  init_shell();

  static char line[4096];
  int line_num = 0;

  /* Please only print shell prompts when standard input is not a tty */
  if (shell_is_interactive)
    fprintf(stdout, "%d: ", line_num);

  while (fgets(line, 4096, stdin)) {
    /* Split our line into words. */
    struct tokens *tokens = tokenize(line);

    /* Find which built-in function to run. */
    int fundex = lookup(tokens_get_token(tokens, 0));

    if (fundex >= 0) {
      cmd_table[fundex].fun(tokens);
    } 
    else 
    {
    	/* REPLACE this to run commands as programs. */
    	// fprintf(stdout, "This shell doesn't know how to run programs.\n");

		char *execFile = (char *)malloc(sizeof(char)*4096);
		char *outFile = (char *)malloc(sizeof(char)*4096);
		
		// If > is present
		bool redirection = false;
		bool pipe = false;
		int output = 1;

		int i = 0;
		for(;i<4096;i++)
		{
			if(line[i]=='|')
			{
		  		pipe = true;
		  		break;
			}
		}

		if(pipe)
		{
			fprintf(stdout, "pipes are not yet supported\n");
		}
		
		// to read name of execution file
		i = 0;
		for(;i<4096;i++)
		{
			if(line[i]==10 || line[i]==' ' || line[i]=='>')
			{
		  		execFile[i] = '\0';
		  		break;
			}
			execFile[i] = line[i];
		}
		// if user wants to redirect
		for(;i<4096;i++)
		{
			if(line[i]=='>')
			{
		  		redirection = 1;
		  		i++;
		  		break;
			}
		}
		
		if(redirection)
		{
			// stripping initial whitespaces
			while(line[i]==' ')
			{
				i++;
			}
			int startOutputFileIndex = i;
			for(;i<4096;i++)
			{
		  		if(line[i]==10 || line[i]==' ')
		  		{
		    		outFile[i-startOutputFileIndex]='\0';
		    		break;
		  		}
		  		outFile[i-startOutputFileIndex]=line[i];
			}
		}
		
		if(redirection && strlen(outFile))
		{
			output = open(outFile,O_WRONLY | O_CREAT | O_TRUNC);
		}
		else if(redirection && !strlen(outFile))
		{
			fprintf(stdout, "Redirection file not defined\n");
		}

		int pid = fork();
		char *args[] = {execFile, NULL};
		if (pid == 0)
		{
			// child process must use default signal handlers
			signal(SIGINT, SIG_DFL);

			// to ensure that its process group is placed in the foreground.
			tcsetpgrp(0, getpgrp());

			// child process
			int old_stdout = dup(1);
			if(output!=1)
			{ 
				dup2(output, 1);
			}
			execv(args[0], args);
			if(output!=1)
			{
				close(output);
			}
			dup2(old_stdout,1);
			fprintf(stdout, "%s: Command not found. Use ? for help.\nNOTE To run a executable in same directory make sure you use './<executable>'\n", args[0]);
			return 0;
		}
		else 
		{
			// parent will wait for the child process to complete
			wait(NULL);
		}
			
	}

    if (shell_is_interactive)
    {
      /* Please only print shell prompts when standard input is not a tty */
      fprintf(stdout, "%d: ", ++line_num);
  	}

    /* Clean up memory */
    tokens_destroy(tokens);
  }

  return 0;
}
