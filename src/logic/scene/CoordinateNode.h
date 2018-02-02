#ifndef __SL_FRAMEWORK_COORDINATENODE_H__
#define __SL_FRAMEWORK_COORDINATENODE_H__
class CoordinateSystem;
class CoordinateNode{
public:
    enum Coordinate{
        X = 1,
        Y = 2,
        Z = 3,
    };
    CoordinateNode(IObject* object);
    ~CoordiateNode();

    inline float x() const {return _x;}
    inline float y() const {return _y;}
    inline float z() const {return _z;}

    inline float oldx() const {return _oldx;}
    inline float oldy() const {return _oldy;}
    inline float oldz() const {return _oldz;}

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
    
    inline CoordinateNode* prevNode(Coordinate type){
        return type == Coordinate::X ? prevX() : (type == Coordinate::Y ? prevY() : prevZ());
    }

    inline CoordinateNode* nextNode(Coordinate type){
        return type == Coordinate::X ? nextX() : (type == Coordinate::Y ? nextY() : nextZ());
    }

    inline void setPrevNode(CoordinateNode* prevNode, Coordinate type){
        type == Coordinate::X ? setPrevX(prevNode) : (type == Coordinate::Y ? setPrevY(prevNode) : setPrevZ(prevNode));
    }

    inline void setNextNode(CoordinateNode* nextNode, Coordinate type){
        type == Coordinate::X ? setNextX(nextNode) : (type == Coordinate::Y ? setNextY(nextNode) : setNextZ(nextNode));
    }

    void resetOld(){
        _oldx = xx();
        _oldy = yy();
        _oldz = zz();
    }

    void setCoordinateSystem(CoordinateSystem* sys);
    inline CoordinateSystem* CoordinateSystem() const {return _coordinateSystem;}


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
};
#endif
