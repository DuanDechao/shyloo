#ifndef __SL_PUBLIC_OBJECT_ALLOCATOR_H__
#define __SL_PUBLIC_OBJECT_ALLOCATOR_H__
#include "slmulti_sys.h"
#include <deque>
namespace sl{
template<typename T>
class SLOjbectAllocator{
public:
	typedef std::deque<T*> ObjectQueue;

	SLOjbectAllocator(int32 initCount = 0)
		:_initCount(initCount),
		_allocCount(0)
	{}

	~SLOjbectAllocator(){
		for (typename ObjectQueue::iterator itr = _objects.begin(); itr != _objects.end(); ++itr){
			DEL (*itr);
		}
		_objects.clear();
	}

	template<typename... Args>
	inline void init(Args... args){
		for (int32 i = 0; i < _initCount; i++){
			++_allocCount;
			T * t = NEW T(args...);
			_objects.push_back(t);
		}
	}

	template<typename... Args>
	inline T* allocate(Args... args){
		if (_objects.empty()){
			++_allocCount;
			return NEW T(args...);
		}
		else{
			T* t = _objects.front();
			_objects.pop_front();
			return t;
		}
	}

	inline void deallocate(T* p){
		_objects.push_back(p);
	}

private:
	ObjectQueue _objects;
	int32		_initCount;
	int32		_allocCount;
};
}
#endif