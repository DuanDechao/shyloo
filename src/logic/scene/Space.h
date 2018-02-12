#ifndef __SL_FRAMEWORK_SPACE_H__
#define __SL_FRAMEWORK_SPACE_H__
#include <vector>
#include "slmulti_sys.h"

class IObject;
class Space{
public:
    Space(const int32 spaceId);
    ~Space();

    void addObject(IObject* object);
private:
    int32                   _id;
    std::vector<IObject*>   _spaceObjects;
};
#endif
