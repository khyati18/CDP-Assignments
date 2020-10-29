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
using namespace std;

// locks[0]:v, locks[1]:w, locks[2]:x, locks[3]:y, locks[4]:z 
vector <struct_lock> locks(5);

typedef struct state_variable
{
	int v;
	int w;
	int x;
	int y;
	int z;
}
state_var;

int total_transactions;
state_var data;
vector <transaction> trans;

/////*********LockMgr******///////
sem_t mutex_global;
void acquire_Read_lock(int transId, int varName) 
{
	sem_wait(&locks[varName].lock);
	sem_wait(&mutex_global);
	trans[transId-1].ac_lock[varName]=true;
    cout << "R-lock [" << transId << " , " << char('V' +varName) << "]\n";	
    locks[varName].state = 0;
	sem_post(&mutex_global);
}

void acquire_Write_lock(int transId, int varName) 
{
    sem_wait(&locks[varName].lock);
	sem_wait(&mutex_global);
	trans[transId-1].ac_lock[varName]=true;
    cout << "W-lock [" << transId << " , " <<char( 'V' +varName) << "]\n";
    locks[varName].state = 1;
	sem_post(&mutex_global);
}

void upgrade_to_Write(int transId, int varName)
{ 
	sem_wait(&mutex_global);
    cout << "upgrade [" << transId << " , " << char('V' +varName) << "]\n";
    locks[varName].state = 1;
	sem_post(&mutex_global);
}

void release_lock(int transId, int varName) 
{
    cout << "unlock [" << transId << " , " <<char('V' +varName) << "]\n";
	sem_post(&locks[varName].lock);
    locks[varName].state = 0;
}
void release_all(int transId)
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
	sem_post(&mutex_global);
}
void evaluate(string op)
{
	sem_wait(&mutex_global);
	cout<<"we do some operation on "<<op[0]<<"\n";
	sem_post(&mutex_global);

}

///////****LockMgr End****/////////

void takeinput()
{
	char var;
	
	cin >> total_transactions;
	cin >> var >> data.v;
	cin >> var >> data.w;
	cin >> var >> data.x;
	cin >> var >> data.y;
	cin >> var >> data.z;
	
	trans.resize(total_transactions);
	
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
	
	while(!T.op_seq.empty())
	{
		auto op = T.op_seq.front();
		T.op_seq.pop();


		if(op[0]=='R' )
		{
			acquire_Read_lock(T.id, op[2]-'V');
		}
		else if(op[0]=='W')
		{
			upgrade_to_Write(T.id,op[2]-'V');
		}
		else if(op[0]=='C')
		{
			//cout << "Commiting transaction" << T.id << endl;
			release_all(T.id);
		}
		else if(op[0]=='A')
		{
			//cout << "Aborting transaction" << T.id << endl;
			release_all(T.id);
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
	sem_init(&mutex_global,0,1);
	takeinput();

	/* For running transactions in diff. threads
	*/
	
	vector< thread > threads(total_transactions);
  	for (int i = 0; i < trans.size(); i++) 
  	{
		threads[i] = thread(execute_Transaction,trans[i]);
  	}
  	for (auto &th : threads) 
  	{
    	th.join();
  	}

	return 0;
}