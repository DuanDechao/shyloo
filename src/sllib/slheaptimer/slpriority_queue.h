#ifndef SL_PRIORITY_QUEUE_H
#define SL_PRIORITY_QUEUE_H
#include <vector>
#include <functional>
#include <algorithm>
#include "sltimer_base.h"
namespace sl{namespace timer{
class Comparator{
public:
	bool operator()(const CSLTimerBase* a, const CSLTimerBase* b){
		return a->getExpireTime() > b->getExpireTime(); 
	}
};

class SLPriorityQueue{
public:
	SLPriorityQueue(){}
	~SLPriorityQueue(){}

	inline bool empty() const {return _timerContainer.empty();}
	inline int32 size() const {return _timerContainer.size();}
	inline CSLTimerBase* top() const {return _timerContainer.front();}
	inline std::vector<CSLTimerBase*>& getContainer() {return _timerContainer;}

	void push(CSLTimerBase* timer){
		_timerContainer.push_back(timer);
		std::push_heap(_timerContainer.begin(), _timerContainer.end(), Comparator());
	}

	void pop(){
		std::pop_heap(_timerContainer.begin(), _timerContainer.end(), Comparator());
		_timerContainer.pop_back();
	}

	void makeHeap(){
		std::make_heap(_timerContainer.begin(), _timerContainer.end(), Comparator());
	}


private:
	std::vector<CSLTimerBase*> _timerContainer;
	int32	_numPurged;
};

}
}
#endif
