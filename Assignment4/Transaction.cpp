#include<bits/stdc++.h> 
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

class Transaction
{
	public:

		int transaction_id;
		int status;

		queue <pair<operation, variable>> sequenec_of_operations;

		// status = 0 for abort
		// status = 1 for commit
};