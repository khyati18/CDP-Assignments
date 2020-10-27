#include<bits/stdc++.h> 
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
using namespace std;
class Transaction
{
	public:

		int transaction_id;
		int status;

		queue <string> op_seq;

		// status = 0 for abort
		// status = 1 for commit
};