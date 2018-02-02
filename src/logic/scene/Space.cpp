#include "Space.h"
Space::Space(const int32 spaceId)
    :_id(spaceId)
{}

Space::~Space(){}


void Space::addObject(IObject* object){
    _spaceObjects.push_back(object);    
}
