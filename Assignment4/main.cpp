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
sem_t mutex_global;

void acquire_Read_lock(int transId, int varName) 
{

	//sem_wait(&mutex_global);
    cout << "R-lock [" << transId << " , " << char('V' +varName) << "]\n";
	sem_wait(&locks[varName].lock);
    locks[varName].state = 0;
	//sem_post(&mutex_global);
}

void acquire_Write_lock(int transId, int varName) 
{
     
	//sem_wait(&mutex_global);
    cout << "W-lock [" << transId << " , " <<char( 'V' +varName) << "]\n";
	sem_wait(&locks[varName].lock);
    locks[varName].state = 1;
	//sem_post(&mutex_global);
}

void upgrade_to_Write(int transId, int varName)
{ 
	//sem_wait(&mutex_global);
    cout << "upgrade [" << transId << " , " << char('V' +varName) << "]\n";
    locks[varName].state = 1;
	//sem_post(&mutex_global);
}

void release_lock(int transId, int varName) 
{
	
	//sem_wait(&mutex_global);
    cout << "unlock [" << transId << " , " <<char('V' +varName) << "]\n";
	sem_post(&locks[varName].lock);
    locks[varName].state = 1;
	//sem_post(&mutex_global);

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

int total_transactions;
state_var data;
vector <transaction> trans;

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
			trans[i].status = 0;
		}
		else 
		{
			trans[i].status = 1;
		}	
	}
}

void execute_Transaction(transaction T)
{
	//cout << "Starting to execute transaction - " << T.id << "\n";
	
	// acquire_Read_lock(T, variable);
	// Update state variable 
	// release_lock(T, variable);
	
	while(!T.op_seq.empty())
	{
		sem_wait(&mutex_global);
		auto op = T.op_seq.front();
		T.op_seq.pop();
		cout << op << endl;

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
			T.status = 1;
		}
		else if(op[0]=='A')
		{
			T.status = 0;
		}
		else
		{
			//sem_wait(&mutex_global);
			cout<<"we do some operation on "<<op[0]<<"\n";
			//sem_post(&mutex_global);
		}
		sem_post(&mutex_global);
		
	}
	
	if(T.status==1)
	{
		sem_wait(&mutex_global);
		cout << "Commiting transaction" << T.id << endl;
		sem_post(&mutex_global);
	}
	else
	{
		sem_wait(&mutex_global);
		cout << "Aborting transaction" << T.id << endl;
		sem_post(&mutex_global);
	}
	

	return;
}




int main()
{
	sem_init(&mutex_global,0,1);
	cout<<1<<endl;

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