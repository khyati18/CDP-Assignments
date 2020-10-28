#include<bits/stdc++.h> 
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "LockMgr.cpp"
#include "transaction.hpp"
using namespace std;



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
		threads[i]=thread(execute_Transaction,trans[i]);
  	}
  	for (auto &th : threads) 
  	{
    	th.join();
  	}
  	

	return 0;
}