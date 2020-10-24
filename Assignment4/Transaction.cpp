#include<bits/stdc++.h> 

class Transaction
{
	public:

		int transaction_id;
		int status;

		queue <pair<operation, variable>> sequenec_of_operations;

		// status = 0 for abort
		// status = 1 for commit

		Transaction();
		~Transaction();
	

};