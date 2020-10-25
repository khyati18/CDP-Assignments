#include<bits/stdc++.h> 
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

using namespace std;

void execute_Transaction(Transaction T)
{
	cout << "Starting to execute transaction - \n";
	acquire_Read_lock(T, variable);
	// Update state variable 
	release_lock(T, variable);
	// print commit/abort
	return;
}

int main(int argc, char const *argv[])
{
	// state variables
	int U = 0;
	int V = 0;
	int X = 0;
	int Y = 0;
	int Z = 0;	

	Transaction T[] = {};
	// take input file


	// execute transaction in different threads
	std::vector<std::thread> threads;
 
  	for (int i = 0; i < transactions.size(); i++) 
  	{
    	threads.push_back(std::thread(execute_Transaction, T[i]));
  	}
 
  	for (auto &th : threads) 
  	{
    	th.join();
  	}

	return 0;
}