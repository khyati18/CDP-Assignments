#include<bits/stdc++.h> 


using namespace std;

class transaction
{
	public:

		int id;
		int status;		// status = 0 for abort & status = 1 for commit

		queue <string> op_seq;		// sequence of operations queue
};