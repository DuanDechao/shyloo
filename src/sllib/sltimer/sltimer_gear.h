#ifndef SL_SLTIMER_GEAR_H
#define SL_SLTIMER_GEAR_H
#include "sltimer.h"
#include "sltimer_base.h"
#include "slsingleton.h"
#include "sltimer_base.h"
namespace sl{
namespace timer{
class SLTimerGear{
public:
	SLTimerGear(int32 maxSlotNum, SLTimerGear* nextGear);
	~SLTimerGear();

	void checkHighGear();
	void updateToLowGear();
	void update();

public:
	SLList*			_slots;
	SLTimerGear*	_nextGear;
	int32			_nowSlot;
	int32			_maxSlotNum;

private:
	SLTimerGear(const SLTimerGear&);
	SLTimerGear& operator = (const SLTimerGear&);
};

}
}
#endif