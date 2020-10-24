#include<bits/stdc++.h> 

class LockMgr
{
	public:

		Lock lock_variable;
		Condition_variable cv[100];

		queue <Transaction> trans_queue;

		LockMgr();
		~LockMgr();

		void acquire_Read_lock(int transId, int varName)
		{

		}

		void acquire_Write_lock(int transId, int varName)
		{

		}

		void upgrade_to_Write(int transId, int varName)
		{

		}

		void release_lock(int transId, int varName)
		{

		}
	
};