#ifndef _SL_MUTEX_H_
#define _SL_MUTEX_H_
#include "slmulti_sys.h"
#include <atomic>
namespace sl{
class spin_mutex{
	std::atomic_flag _flag;
public:
#ifdef SL_OS_WINDOWS
	spin_mutex(){ _flag._My_flag = 0; }
#elif defined SL_OS_LINUX
	spin_mutex():_flag(ATOMIC_FLAG_INIT){}
#endif // SL_OS_WINDOWS
	spin_mutex(const spin_mutex&) = delete;
	spin_mutex& operator = (const spin_mutex&) = delete;

	bool try_lock(){
		return !_flag.test_and_set(std::memory_order_acquire);
	}

	void lock(){
		while (_flag.test_and_set(std::memory_order_acquire));
	}

	void unlock(){
		_flag.clear(std::memory_order_release);
	}
};
}


#endif