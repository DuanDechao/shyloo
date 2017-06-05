#include "sltimer_gear.h"
namespace sl{
namespace timer{
SLTimerGear::SLTimerGear(int32 maxSlotNum, SLTimerGear* nextGear)
	:_nowSlot(0),
	_maxSlotNum(maxSlotNum),
	_nextGear(nextGear),
	_slots(nullptr)
{
	_slots = NEW SLList[maxSlotNum];
}

SLTimerGear::~SLTimerGear(){
	_nowSlot = 0;
	_maxSlotNum = 0;
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


}
}