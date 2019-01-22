#ifndef __SL_FRAMEWORK_COORDINATESYSTEM_H__
#define __SL_FRAMEWORK_COORDINATESYSTEM_H__
#include <list>
#include "CoordinateNode.h"

class CoordinateSystem{
public:
    CoordinateSystem();
    ~CoordinateSystem();


    bool insert(CoordinateNode* pNode);

    bool remove(CoordinateNode* pNode);
    bool removeReal(CoordinateNode* pNode);
    void removeDelNodes();
    void releaseNodes();

    void update(CoordinateNode* pNode);

    inline CoordinateNode* xNodeHead() const {return _xCoordinateHead;}
    inline CoordinateNode* yNodeHead() const {return _yCoordinateHead;}
    inline CoordinateNode* zNodeHead() const {return _zCoordinateHead;}

    inline bool isEmpty() const {return _xCoordinateHead == NULL && _yCoordinateHead == NULL && _zCoordinateHead == NULL;}    
    inline uint32 size() const {return _size;}

    void moveNodeX(CoordinateNode* pNode, float px, CoordinateNode* pCurrNode);
    void moveNodeY(CoordinateNode* pNode, float py, CoordinateNode* pCurrNode);
    void moveNodeZ(CoordinateNode* pNode, float pz, CoordinateNode* pCurrNode);
    void debugNodes();
private:
    uint32 _size;
    CoordinateNode* _xCoordinateHead;
    CoordinateNode* _yCoordinateHead;
    CoordinateNode* _zCoordinateHead;
    
    std::list<CoordinateNode*> _dels;
    std::list<CoordinateNode*> _releases;
    size_t _delCount;
     


};
#endif
