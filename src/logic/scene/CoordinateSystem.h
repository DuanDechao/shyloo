#ifndef __SL_FRAMEWORK_COORDINATESYSTEM_H__
#define __SL_FRAMEWORK_COORDINATESYSTEM_H__
class CoordinateSystem{
public:
    CoordinateSystem();
    ~CoordinateSystem();


    bool insert(CoordinateNode* pNode);

    bool remove(CoordinateNode* pNode);

    void moveNodeX(CoordinateNode* pNode, float x, CoordinateNode* pCurrNode);
    void moveNodeY(CoordinateNode* pNode, float y, CoordinateNode* pCurrNode);
    void moveNodeZ(CoordinateNode* pNode, float z, CoordinateNode* pCurrNode);

    void update(CoordinateNode* pNode);

    inline CoordinateNode* xNodeHead() const {return _xCoordinateHead;}
    inline CoordinateNode* yNodeHead() const {return _yCoordinateHead;}
    inline CoordinateNode* zNodeHead() const {return _zCoordinateHead;}

    inline bool isEmpty() const {return _xCoordinateHead == NULL && _yCoordinateHead == NULL && _zCoordinateHead == NULL;}    
    
    void moveNodeX(CoordinateNode* pNode, float px, CoordinateNode* pCurrNode);

private:
    CoordinateNode* _xCoordinateHead;
    CoordinateNode* _yCoordinateHead;
    CoordinateNode* _zCoordinateHead; 


};
#endif
