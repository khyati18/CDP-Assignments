#include<bits/stdc++.h> 
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

// Reference : http://willzhang4a58.github.io/2016/07/rwlock/

class LockMgr
{
	public:
	   
	    LockMgr() : _status(0), _waiting_readers(0), _waiting_writers(0) {}
	    LockMgr(const LockMgr&) = delete;
	    LockMgr(LockMgr&&) = delete;
	    LockMgr& operator = (const LockMgr&) = delete;
	    LockMgr& operator = (LockMgr&&) = delete;

	    void acquire_Read_lock(int transId, int varName) 
	    {
	        std::unique_lock<std::mutex> lck(_mtx);
	        _waiting_readers += 1;
	        _read_cv.wait(lck, [&]() { return _waiting_writers == 0 && _status >= 0; });
	        _waiting_readers -= 1;
	        _status += 1;
	    }

	    void acquire_Write_lock(int transId, int varName) 
	    {
	        std::unique_lock<std::mutex> lck(_mtx);
	        _waiting_writers += 1;
	        _write_cv.wait(lck, [&]() { return _status == 0; });
	        _waiting_writers -= 1;
	        _status = -1;
	    }

	    void upgrade_to_Write(int transId, int varName)
		{
			return;
		}

	    void release_lock(int transId, int varName) 
	    {
	        std::unique_lock<std::mutex> lck(_mtx);
	        if (_status == -1) {
	            _status = 0;
	        } else {
	            _status -= 1;
	        }
	        if (_waiting_writers > 0) {
	            if (_status == 0) {
	                _write_cv.notify_one();
	            }
	        } else {
	            _read_cv.notify_all();
	        }
	    }

	private:
	    // -1    : one writer
	    // 0     : no reader and no writer
	    // n > 0 : n reader
	    int32_t _status;
	    int32_t _waiting_readers;
	    int32_t _waiting_writers;
	    std::mutex _mtx;
	    std::condition_variable _read_cv;
	    std::condition_variable _write_cv;
};