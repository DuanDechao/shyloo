#ifndef SLLIB_SLXML_H
#define SLLIB_SLXML_H
#include "slmulti_sys.h"
namespace sl
{
namespace xml
{
class ISLXmlNode{
public:
	virtual int8 SLAPI getAttributeInt8(const char* name) = 0;
	virtual int16 SLAPI getAttributeInt16(const char* name) = 0;
	virtual int32 SLAPI getAttributeInt32(const char* name) = 0;
	virtual int64 SLAPI getAttributeInt64(const char* name) = 0;
	virtual const char* SLAPI getAttributeString(const char* name) = 0;
	virtual float SLAPI getAttributeFloat(const char* name) = 0;
	virtual ISLXmlNode* operator[] (const char* nodeName) = 0;
};

class ISLXmlBase{
public:
	virtual ISLXmlNode* SLAPI root() = 0;
};

ISLXmlBase* SLAPI loadXmlFile(const char* path);
}
}
#endif