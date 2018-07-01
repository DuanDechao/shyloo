#ifndef __SL_FRAMEWORK_COORDINATENODE_H__
#define __SL_FRAMEWORK_COORDINATENODE_H__
#include "slmulti_sys.h"
class CoordinateSystem;
class CoordinateNode{
public:
    enum {
        COORDINATE_NODE_FLAG_UNKNOWN            = 0x00000000,
        COORDINATE_NODE_FLAG_OBJECT             = 0x00000001,   //一个Object节点
        COORDINATE_NODE_FLAG_TRIGGER            = 0x00000002,   //一个触发器节点
        COORDINATE_NODE_FLAG_HIDE               = 0x00000004,   //一个隐藏节点
        COORDINATE_NODE_FLAG_REMOVING           = 0x00000008,   //删除中节点
        COORDINATE_NODE_FLAG_REMOVED            = 0x00000010,   //删除节点
        COORDINATE_NODE_FLAG_PENDING            = 0x00000020,   //处在update操作中
        COORDINATE_NODE_FLAG_NODE_UPDATING      = 0x00000040,   //正在执行update操作
        COORDINATE_NODE_FLAG_INSTALLING         = 0x00000080,   //正在安装操作
        COORDINATE_NODE_FLAG_POSITIVE_BOUNDARY  = 0x00000100,   //节点是触发器正边界
        COORDINATE_NODE_FLAG_NEGATIVE_BOUNDARY  = 0x00000200,   //节点是触发器的负边界
    };


    CoordinateNode(CoordinateSystem* pCoordinateSystem = NULL);
    virtual ~CoordinateNode();

    inline void setFlags(uint32 v) {_flags = v;}
    inline uint32 flags() const {return _flags;}
    inline void addFlags(uint32 v) {_flags |= v;}
    inline void removeFlags(uint32 v) {_flags &= ~v;}
    inline bool hasFlags(uint32 v) const {return (_flags & v) > 0; }

    inline float x() const {return _x;}
    inline float y() const {return _y;}
    inline float z() const {return _z;}

    inline float oldX() const {return _oldx;}
    inline float oldY() const {return _oldy;}
    inline float oldZ() const {return _oldz;}

    inline void setX(float x) {_x = x;}
    inline void setY(float y) {_y = y;}
    inline void setZ(float z) {_z = z;}

    inline void setOldX(float oldx) {_oldx = oldx;}
    inline void setOldY(float oldy) {_oldy = oldy;}
    inline void setOldZ(float oldz) {_oldz = oldz;}

    virtual float xx() const { return 0.f;}
    virtual float yy() const { return 0.f;}
    virtual float zz() const { return 0.f;}

    inline CoordinateNode* prevX() const {return _prevX;} 
    inline CoordinateNode* prevY() const {return _prevY;} 
    inline CoordinateNode* prevZ() const {return _prevZ;} 
    inline CoordinateNode* nextX() const {return _nextX;} 
    inline CoordinateNode* nextY() const {return _nextY;} 
    inline CoordinateNode* nextZ() const {return _nextZ;}

    inline void setPrevX(CoordinateNode* prevX) {_prevX = prevX;} 
    inline void setPrevY(CoordinateNode* prevY) {_prevY = prevY;} 
    inline void setPrevZ(CoordinateNode* prevZ) {_prevZ = prevZ;} 
    inline void setNextX(CoordinateNode* nextX) {_nextX = nextX;} 
    inline void setNextY(CoordinateNode* nextY) {_nextY = nextY;} 
    inline void setNextZ(CoordinateNode* nextZ) {_nextZ = nextZ;} 
    
    virtual void onNodePassX(CoordinateNode* pNode){}
    virtual void onNodePassY(CoordinateNode* pNode){}
    virtual void onNodePassZ(CoordinateNode* pNode){}

    virtual void onRemove();
    
    virtual void update();

    virtual void resetOld(){
        _oldx = xx();
        _oldy = yy();
        _oldz = zz();
    }

    void setCoordinateSystem(CoordinateSystem* sys);
    inline CoordinateSystem* coordinateSystem() const {return _coordinateSystem;}


protected:
    CoordinateNode*     _prevX;
    CoordinateNode*     _nextX;
    CoordinateNode*     _prevY;
    CoordinateNode*     _nextY;
    CoordinateNode*     _prevZ;
    CoordinateNode*     _nextZ;

    CoordinateSystem*   _coordinateSystem;
    
    float   _x, _y, _z;
    float   _oldx, _oldy, _oldz;

    uint32  _flags;
};
#endif
