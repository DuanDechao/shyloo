#ifndef __SL_PUBLIC_CYCLE_QUEUE_H__
#define __SL_PUBLIC_CYCLE_QUEUE_H__
#include "slmulti_sys.h"
namespace sl
{
template <typename T>
class CycleQueue{
public:
	CycleQueue(const int32 size){
		_front = 0;
		_tail = 0;
		_size = size + 1;
		SLASSERT(_size > 0, "size is invaild");
		
		_queue = (T*)SLMALLOC(_size * sizeof(T));
	}

	~CycleQueue(){
		SLFREE(_queue);
	}

	inline bool read(T& t){
		if (isEmpty())
			return false;

		t = _queue[_front];
		_front = (_front + 1) % _size;
		return true;
	}

	inline void push(const T& t){
		while (isFull()){
			CSLEEP(1);
		}

		_queue[_tail] = t;
		_tail = (_tail + 1) % _size;
	}

	inline bool isEmpty(){
		return _front == _tail;
	}

	inline bool isFull() {
		return _front == (_tail + 1) % _size;
	}

private:
	T * _queue;
	volatile int32 _front;
	volatile int32 _tail;
	int32 _size;
};

}
#endif