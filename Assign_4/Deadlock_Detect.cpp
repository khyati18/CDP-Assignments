// reference - https://faouellet.github.io/ddmutex/

#include<bits/stdc++.h> 
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp> 
#include <thread>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/connected_components.hpp>

using ThreadID = std::thread::id;
using namespace std::chrono_literals;

class WaitForGraph
{
	private:
		using GraphT = boost::labeled_graph<
	                   boost::adjacency_list< 					// Graph representation
	                   boost::vecS,      						// Edge storage
	                   boost::vecS,   						    // Vertex storage
	                   boost::directedS>, 						// Directed graph
	                   ThreadID>; 								// Vertex label
	private:
		std::mutex mMutex;
		GraphT mGraph;
	
	public:
		static WaitForGraph& get()
		{
			static WaitForGraph graph;
			return graph;
		}

	public:
		WaitForGraph() = default;
	  	WaitForGraph(const WaitForGraph&) = delete;
	  	WaitForGraph& operator=(const WaitForGraph&) = delete;
	  	WaitForGraph(WaitForGraph&&) = delete;
	  	WaitForGraph& operator=(WaitForGraph&&) = delete;

	public:
	  	void AddNode(const ThreadID& tid)
	  	{
	    	std::lock_guard<std::mutex> lg{mMutex};
	    	boost::add_vertex(tid, mGraph);
	  	}

	  	void RemoveNode(const ThreadID& tid)
	  	{
	    	std::lock_guard<std::mutex> lg{mMutex};
	    	boost::clear_vertex_by_label(tid, mGraph);
	 	}

	public:
		void AddEdge(const ThreadID& fromTID, const ThreadID& toTID)
	  	{
	  		std::lock_guard<std::mutex>{mMutex};
	  		boost::add_edge_by_label(fromTID, toTID, mGraph);

	  		std::vector<int> sccs(boost::num_vertices(mGraph));
	  		if (boost::connected_components(mGraph, sccs.data()) != 0)
	  		{
	  			std::cerr << "DEADLOCK DETECTED:\n" << "Thread 1 ID: " << fromTID << "\n" << "Thread 2 ID: " << toTID << "\n";
	        }
		}
};

class DDMutex
{
	private:
		std::mutex mMutex;
		ThreadID mOwnerID;
	  	bool mIsOwned = false;

	public:
		void Lock()
		{
			ThreadID currTID = std::this_thread::get_id();
			WaitForGraph::get().AddNode(currTID);

			if (mIsOwned)
			{
				WaitForGraph::get().AddEdge(mOwnerID, currTID);
			}

			mMutex.lock();
			mOwnerID = currTID;
			mIsOwned = true;
		}

	public:
		void Unlock()
		{
			WaitForGraph::get().RemoveNode(mOwnerID);
			mIsOwned = false;
			mMutex.unlock();
		}
};

int main()
{
	DDMutex m1, m2;

	std::vector<std::thread> threads;

	threads.emplace_back([&m1, &m2]()
	{
		m1.Lock();
		m2.Lock();
		std::this_thread::sleep_for(1s);
		m1.Unlock();
		m2.Unlock();
	});

	threads.emplace_back([&m1, &m2]()
	{
		m2.Lock();
		m1.Lock();
		std::this_thread::sleep_for(1s);
		m2.Unlock();
		m1.Unlock();
	});

	for (auto& th : threads)
	{
		if (th.joinable())
		{
	  	th.join();
		}
	}

	return 0;
}