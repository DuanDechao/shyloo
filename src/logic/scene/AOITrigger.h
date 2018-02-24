#ifndef __SL_FRAMEWORK_AOITRIGGER_H__
#define __SL_FRAMEWORK_AOITRIGGER_H__

#include "RangeTrigger.h"
class Witness;
class AOITrigger: public RangeTrigger{
public:
    AOITrigger(CoordinateNode* origin, float xz = 0.0f, float y = 0.0f);
    virtual ~AOITrigger();

    virtual void onEnter(CoordinateNode* pNode);
    virtual void onLeave(CoordinateNode* pNode);

    inline Witness* getWitness() const {return _witness;}

protected:
    Witness* _witness;
};
#endif
