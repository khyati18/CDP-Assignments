#include<bits/stdc++.h> 
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "LockMgr.cpp"

using namespace std;

class transaction
{
	public:

		int id;
		int status;		// status = 0 for abort & status = 1 for commit

		queue <string> op_seq;		// sequence of operations queue
};

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

	for (int i = 0; i < total_transactions; ++i)
	{
		execute_Transaction(trans[i]);
	}

	/* For running transactions in diff. threads

	vector< thread > threads;
  	for (int i = 0; i < trans.size(); i++) 
  	{
		//threads[i](execute_Transaction, trans[i]);
    	threads.push_back(thread(execute_Transaction, trans[i]));
  	}
	cout<<0<<endl;
  	for (auto &th : threads) 
  	{
    	th.join();
  	}
  	*/
	
	return 0;
}