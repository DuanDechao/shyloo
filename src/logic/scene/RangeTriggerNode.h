#ifndef __SL_FRAMEWORK_RANGETRIGGERNODE_H__
#define __SL_FRAMEWORK_RANGETRIGGERNODE_H__
#include "CoordinateNode.h"
#include "RangeTrigger.h"

#include <math.h>
class RangeTriggerNode: public CoordinateNode{
public:
    RangeTriggerNode(RangeTrigger* pRangeTrigger, float xz, float y, bool positiveBoundary);
    virtual ~RangeTriggerNode();

    inline void setRange(float xz, float y) {_rangeXZ = xz; _rangeY = y;}
    inline void setOldRange(float xz, float y) {_oldRangeXZ = xz; _oldRangeY = y;}
    inline float rangeXZ() const { return _rangeXZ;}
    inline float rangeY() const {return _rangeY;}

    inline RangeTrigger* rangeTrigger() const {return _rangeTrigger;}
    inline void setRangeTrigger(RangeTrigger* pRangeTrigger){_rangeTrigger = pRangeTrigger;}

    virtual float xx() const;
    virtual float yy() const;
    virtual float zz() const;


    inline bool isInXRange(CoordinateNode* pNode){
        float originX = _rangeTrigger->origin()->xx();
        volatile float lowerBound = originX - fabs(_rangeXZ);
        volatile float upperBound = originX + fabs(_rangeXZ);
        
        return (pNode->xx() >= lowerBound) && (pNode->xx() <= upperBound);
    }

    inline bool isInYRange(CoordinateNode* pNode){
        float originY = _rangeTrigger->origin()->yy();
        volatile float lowerBound = originY - fabs(_rangeY);
        volatile float upperBound = originY + fabs(_rangeY);

        return (pNode->yy() >= lowerBound) && (pNode->yy() <= upperBound);
    }

    inline bool isInZRange(CoordinateNode* pNode){
        float originZ = _rangeTrigger->origin()->zz();
        volatile float lowerBound = originZ - fabs(_rangeXZ);
        volatile float upperBound = originZ + fabs(_rangeXZ);

        return (pNode->zz() >= lowerBound) && (pNode->zz() <= upperBound);
    }
    
    inline bool wasInXRange(CoordinateNode* pNode){
        float originX = oldX() - _oldRangeXZ;
        volatile float lowerBound = originX - fabs(_oldRangeXZ);
        volatile float upperBound = originX + fabs(_oldRangeXZ);

        return (pNode->oldX() >= lowerBound) && (pNode->oldX() <= upperBound);
    }

    inline bool wasInYRange(CoordinateNode* pNode){
        float originY = oldY() - _oldRangeY;
        volatile float lowerBound = originY - fabs(_oldRangeY);
        volatile float upperBound = originY + fabs(_oldRangeY);

        return (pNode->oldY() >= lowerBound) && (pNode->oldY() <= upperBound);
    }

    inline bool wasInZRange(CoordinateNode* pNode){
        float originZ = oldZ() - _oldRangeXZ;
        volatile float lowerBound = originZ - fabs(_oldRangeXZ);
        volatile float upperBound = originZ + fabs(_oldRangeXZ);

        return (pNode->oldZ() >= lowerBound) && (pNode->oldZ() <= upperBound);
    }

    virtual void resetOld(){
        CoordinateNode::resetOld();
        _oldRangeXZ = _rangeXZ;
        _oldRangeY = _rangeY;
    }

    virtual void onRemove();

    void onTriggerUninstall();

    virtual void onNodePassX(CoordinateNode* pNode);
    virtual void onNodePassY(CoordinateNode* pNode);
    virtual void onNodePassZ(CoordinateNode* pNode);


protected:
    float _rangeXZ, _rangeY, _oldRangeXZ, _oldRangeY;
    RangeTrigger*    _rangeTrigger;
};
#endif
