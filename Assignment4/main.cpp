#include<bits/stdc++.h> 
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>


using namespace std;


class transaction
{
	public:

		int id;
		int status;

		queue <string> op_seq;

		// status = 0 for abort
		// status = 1 for commit
};
typedef struct vars{
	int v,w,x,y,z;
}vars1;

int tot;
vars1 data;
vector <transaction> trans;
void takeinput()
{
	cin>>tot;

	cin>>data.v>>data.w>>data.x>>data.y>>data.z;
	trans.resize(tot);
	for(int i=0;i<tot;i++)
	{
		cin>>trans[i].id;
		string s;
		cin>>s;
		while(s!="C" && s!="A")
		{
			trans[i].op_seq.push(s);
			cin>>s;
		}
		if(s=="A")
			trans[i].status=1;
		else 
			trans[i].status=0;
		
	}
}

void execute_Transaction(transaction T)
{
	cout << "Starting to execute transaction - \n";
	//acquire_Read_lock(T, variable);
	cout<<T.status<<endl;
	// Update state variable 
	//release_lock(T, variable);
	// print commit/abort
	return;
}


int main(int argc, char const *argv[])
{

	takeinput();

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

	
	return 0;
}