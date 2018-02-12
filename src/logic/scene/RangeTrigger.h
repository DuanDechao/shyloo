#ifndef __SL_FRAMEWORK_RANGETRIGGER_H__
#define __SL_FRAMEWORK_RANGETRIGGER_H__
class RangeTriggerNode;
class CoordinateNode;
class RangeTrigger{
public:
    RangeTrigger(CoordinateNode* origin, float xz, float y);
    virtual ~RangeTrigger();
    
    bool install();
    bool uninstall();
    bool reinstall(CoordinateNode* pCoordinateNode);

    inline bool isInstalled() const {return _removing;}

    inline void setRange(float xz, float y) { _rangeXZ = xz; _rangeY = y;}
    inline float rangeXZ() const {return _rangeXZ;}
    inline float rangeY() const {return _rangeY;}

    inline CoordinateNode* origin() const {return _origin;}

    inline void setOrigin(CoordinateNode* pCoordinateNode) {_origin = pCoordinateNode;}

    virtual void update(float xz, float y);

    virtual void onEnter(CoordinateNode* pNode) = 0;
    virtual void onLeave(CoordinateNode* pNode) = 0;

    virtual void onNodePassX(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode);
    virtual void onNodePassY(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode);
    virtual void onNodePassZ(RangeTriggerNode* pRangeTriggerNode, CoordinateNode* pNode);


protected:
    CoordinateNode* _origin;
    float _rangeXZ, _rangeY;
    RangeTriggerNode* _positiveBoundary;
    RangeTriggerNode* _negativeBoundary;
    bool _removing;
};
#endif
