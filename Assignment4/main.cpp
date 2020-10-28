#include<bits/stdc++.h> 
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "transaction.hpp"
#include "struct_lock.hpp"
#include <semaphore.h>
using namespace std;

// locks[0]:v, locks[1]:w, locks[2]:x, locks[3]:y, locks[4]:z 
vector <struct_lock> locks(5);

void acquire_Read_lock(int transId, int varName) 
{
    cout << "Acquring read lock on " << varName << "for transaction " << transId << "\n";

    locks[varName].state = 0;
}

void acquire_Write_lock(int transId, int varName) 
{
    return;
}

void upgrade_to_Write(int transId, int varName)
{
	return;
}

void release_lock(int transId, int varName) 
{
	return;
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
	cout << "Starting to execute transaction - " << T.id << "\n";
	
	// acquire_Read_lock(T, variable);
	// Update state variable 
	// release_lock(T, variable);

	while(!T.op_seq.empty())
	{
		auto op = T.op_seq.front();
		T.op_seq.pop();
		cout << op << endl;

		if(op[0]=='R' && op[2]=='V')
		{
			acquire_Read_lock(T.id, 0);
		}
		else if(op[0]=='R' && op[2]=='W')
		{
			acquire_Read_lock(T.id, 1);
		}
		else if(op[0]=='R' && op[2]=='X')
		{
			acquire_Read_lock(T.id, 2);
		}
		else if(op[0]=='R' && op[2]=='Y')
		{
			acquire_Read_lock(T.id, 3);
		}
		else if(op[0]=='R' && op[2]=='Z')
		{
			acquire_Read_lock(T.id, 4);
		}
		else if(op[0]=='C')
		{
			T.status = 1;
		}
		else if(op[0]=='A')
		{
			T.status = 0;
		}
	}

	if(T.status==1)
	{
		cout << "Commiting transaction" << T.id << endl;
	}
	else
	{
		cout << "Aborting transaction" << T.id << endl;
	}

	return;
}




int main()
{

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