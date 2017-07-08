#include "sltimer_gear.h"
#include "sltimer_mgr.h"
namespace sl{
namespace timer{
SLTimerGear::SLTimerGear(int32 maxSlotNum, SLTimerGear* nextGear)
	:_slots(nullptr),
	_nextGear(nextGear),
	_nowSlot(0),
	_maxSlotNum(maxSlotNum)
{
	_slots = NEW SLList[maxSlotNum];
}

SLTimerGear::~SLTimerGear(){
	_nowSlot = 0;
	_maxSlotNum = 0;
	_nextGear = nullptr;
	if (_slots){
		DEL[] _slots;
		_slots = nullptr;
	}
}

void SLTimerGear::checkHighGear(){
	if (_nowSlot >= _maxSlotNum)
		_nowSlot = 0;

	if (_nowSlot == 0 && _nextGear)
		_nextGear->updateToLowGear();
}

void SLTimerGear::updateToLowGear(){
	if (_nowSlot >= _maxSlotNum)
		_nowSlot = 0;

	if (_nowSlot == 0 && _nextGear)
		_nextGear->updateToLowGear();

	SLList* list = &_slots[_nowSlot];
	if (!list){
		SLASSERT(false, "wtf");
		++_nowSlot;
		return;
	}

	SLList tmp;
	list->swap(tmp);
	while (!tmp.isEmpty()){
		CSLTimerBase* pTimer = (CSLTimerBase*)tmp.popFront();
		SLASSERT(pTimer, "wtf");
		SLTimerMgr::getSingletonPtr()->schedule(pTimer);
	}

	++_nowSlot;
}

void SLTimerGear::update(){
	if (_nowSlot >= _maxSlotNum)
		_nowSlot = 0;

	SLList* currList = &_slots[_nowSlot];
	if (!currList){
		SLASSERT(false, "wtf");
		++_nowSlot;
		return;
	}
	SLList temp;
	while (!currList->isEmpty()){
		CSLTimerBase* pTimer = (CSLTimerBase*)currList->popFront();
		SLASSERT(pTimer, "wtf");
		if (!pTimer)
			continue;

		if (pTimer->isPaused()){
			temp.pushBack(pTimer);
			continue;
		}

		SLTimerMgr::getSingletonPtr()->moveToRunList(pTimer);
	}

	while (!temp.isEmpty()){
		currList->pushBack(temp.popFront());
	}

	++_nowSlot;
	if (_nowSlot == _maxSlotNum)
		_nowSlot = 0;

}

}
}
