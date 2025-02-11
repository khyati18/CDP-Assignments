#include<bits/stdc++.h> 
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include<pthread.h>
#include <condition_variable>
#include "transaction.hpp"
#include "struct_lock.hpp"
#include <semaphore.h>
#include<unistd.h>
#include<csignal>
//#include <boost/thread.h>
using namespace std;

// locks[0]:v, locks[1]:w, locks[2]:x, locks[3]:y, locks[4]:z 
vector <struct_lock> locks(5);
vector<bool> wait_state;
int total_transactions;


void detect_deadlock(int signal_num)
{
	cout<<endl;
	cout<<"Dead lock detected between transactions ";
	for(int i=0;i<total_transactions;i++)
	{
		if(wait_state[i]==1)
		{
			cout<<i+1<<" ";
		}
	}
	cout<<endl;

	//sleep(2);
	exit(signal_num);
}
typedef struct state_variable
{
	int v;
	int w;
	int x;
	int y;
	int z;
}
state_var;


state_var data;
vector <transaction> trans;

// Global lock for isolating trasactions
sem_t mutex_global;

// For updating state variable
unordered_map <char, int*> state_vars;

//////////////////////////Lock Manager//////////////////////////////////

void release_lock(int transId, int varName) 
{
    cout << "unlock [" << transId << " , " <<char('V' +varName) << "]\n";
	sem_post(&locks[varName].lock);
    locks[varName].state = 0;
}

void acquire_Read_lock(int transId, int varName) 
{
	sem_wait(&mutex_global);
	int wait_flag = sem_trywait(&locks[varName].lock);
	if(wait_flag==0)
	{
		trans[transId-1].ac_lock[varName]=true;
		cout << "R-lock [" << transId << " , " << char('V' +varName) << "]"<<endl;	
		wait_state[transId-1]=false;
		locks[varName].state = 0;
	}
	//sem_post(&mutex_global);
	if(wait_flag!=0)
	{
		//sem_wait(&mutex_global);
		cout << "Wait [" << transId << " , " << char('V' +varName) << "]"<<endl;
		wait_state[transId-1]=true;
		sem_post(&mutex_global);
		sem_wait(&locks[varName].lock);
		sem_wait(&mutex_global);
	 	trans[transId-1].ac_lock[varName]=true;
		cout << "R-lock [" << transId << " , " << char('V' +varName) << "]"<<endl;	
		wait_state[transId-1]=false;
		locks[varName].state = 0;
		
	}
	sem_post(&mutex_global);
		
	
}

void acquire_Write_lock(int transId, int varName) 
{
	int wait_flag = sem_trywait(&locks[varName].lock);
	if(wait_flag!=0)
	{
		sem_wait(&mutex_global);
		cout << "Wait [" << transId << " , " << char('V' +varName) << "]"<<endl;
		wait_state[transId-1]=true;
		sem_post(&mutex_global);
		sem_wait(&locks[varName].lock);
	}
		
	sem_wait(&mutex_global);
	trans[transId-1].ac_lock[varName]=true;
	cout << "R-lock [" << transId << " , " << char('V' +varName) << "]"<<endl;	
	wait_state[transId-1]=false;
	locks[varName].state = 0;
	sem_post(&mutex_global);
}

void upgrade_to_Write(int transId, int varName)
{ 
	sem_wait(&mutex_global);
	if(!locks[varName].state)
	{
    	cout << "upgrade [" << transId << " , " << char('V' +varName) << "]"<<endl;
	}
    locks[varName].state = 1;
	sem_post(&mutex_global);
}

void release_all(int transId,int f)
{ 
	sem_wait(&mutex_global);
	for(int i=0;i<5;i++)
	{
		if(trans[transId-1].ac_lock[i])
		{
			release_lock(transId,i);
			trans[transId-1].ac_lock[i]=false;
		}	
	}
	if(f)
	{
		cout << "Commit transaction " << transId << endl;
	}
	else 
	{
		cout << "Abort transaction " << transId << endl;
	}
	sem_post(&mutex_global);
}

void evaluate(string op)
{
	sem_wait(&mutex_global);
	cout << "Doing operation " << op << endl;

	if(op.length()==5)													// X=X+Y
	{
		*state_vars[op[0]] = *state_vars[op[2]] + *state_vars[op[4]];
	}
	else if(op.length()==6)												// X=X-50
	{
		*state_vars[op[0]] = *state_vars[op[2]] - 50;
	}
	else if(op.length()==7)												// X=X+100
	{
		*state_vars[op[0]] = *state_vars[op[2]] + 100;
	}
	sem_post(&mutex_global);
}

//////////////////////////Lock Manager//////////////////////////////////

void takeinput()
{
	char var;
	
	cin >> total_transactions;
	cin >> var >> data.v;
	cin >> var >> data.w;
	cin >> var >> data.x;
	cin >> var >> data.y;
	cin >> var >> data.z;

	cout << "Intial Values : V=" << data.v << " W=" << data.w << " X=" << data.x << " Y=" << data.y << " Z=" << data.z << endl;

	state_vars['V'] = &data.v;
	state_vars['W'] = &data.w;
	state_vars['X'] = &data.x;
	state_vars['Y'] = &data.y;
	state_vars['Z'] = &data.z;
	
	trans.resize(total_transactions);
	wait_state.resize(total_transactions,false);
	for(int i=0;i<total_transactions;i++)
	{
		cin >> trans[i].id;
		string s;
		cin >> s;
		while(s!="C" && s!="A")
		{
			trans[i].op_seq.push(s);
			cin >> s;
		}

		if(s=="A")
		{
			trans[i].op_seq.push(s);
			trans[i].status = 0;
		}
		else 
		{
			trans[i].op_seq.push(s);
			trans[i].status = 1;
		}	
		trans[i].ac_lock.resize(5,false);
	}
}

void execute_Transaction(transaction T)
{
	vector <pair<int,bool>> initial_values;

	initial_values.push_back({data.v, false});
	initial_values.push_back({data.w, false});
	initial_values.push_back({data.x, false});
	initial_values.push_back({data.y, false});
	initial_values.push_back({data.z, false});
	
	while(!T.op_seq.empty())
	{
		auto op = T.op_seq.front();
		T.op_seq.pop();


		if(op[0]=='R')
		{
			acquire_Read_lock(T.id, op[2]-'V');
		}
		else if(op[0]=='W' && op.length()==3)
		{
			upgrade_to_Write(T.id,op[2]-'V');
			
			// set true because this variable was updated
			initial_values[op[2]-'V'] = {initial_values[op[2]-'V'].first, true};
		}
		else if(op[0]=='C')
		{
			
			release_all(T.id,1);
		}
		else if(op[0]=='A')
		{
			for (int i = 0; i < 5; ++i)
			{
				if(initial_values[i].second)
				{
					*state_vars['V'+i] = initial_values[i].first;
				}
			}
			release_all(T.id,0);
		}
		else
		{
			evaluate(op);
		}
	}
	return;
}

int main()
{
	signal(SIGINT,detect_deadlock);
	sem_init(&mutex_global,0,1);
	takeinput();
	
	/* For running transactions in diff. threads */
	
	vector< thread > threads(total_transactions);
  	for (int i = 0; i < trans.size(); i++) 
  	{
		threads[i] = thread(execute_Transaction,trans[i]);
  	}
  	for (auto &th : threads) 
  	{
    	th.join();
  	}
	 // th1.join();

  	cout << "Final Values : V=" << data.v << " W=" << data.w << " X=" << data.x << " Y=" << data.y << " Z=" << data.z << endl; 

	return 0;
}