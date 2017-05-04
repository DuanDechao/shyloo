#ifndef SLLIB_SLXML_H
#define SLLIB_SLXML_H
#include "slmulti_sys.h"
#include <vector>
namespace sl
{
namespace xml
{
class ISLXmlNode{
public:
	virtual int8 SLAPI getAttributeInt8(const char* name) const = 0;
	virtual int16 SLAPI getAttributeInt16(const char* name) const = 0;
	virtual int32 SLAPI getAttributeInt32(const char* name) const  = 0;
	virtual int64 SLAPI getAttributeInt64(const char* name) const  = 0;
	virtual const char* SLAPI getAttributeString(const char* name) const = 0;
	virtual float SLAPI getAttributeFloat(const char* name) const = 0;
	virtual bool SLAPI getAttributeBoolean(const char* name) const = 0;
	virtual const ISLXmlNode& SLAPI operator[](const char* nodeName) const = 0;
	virtual const ISLXmlNode& SLAPI operator[](const int32 i) const = 0;
	virtual const int32 SLAPI count(void) const = 0;
	virtual bool SLAPI hasAttribute(const char* name) const = 0;
	virtual bool SLAPI subNodeExist(const char* name) const = 0;
	virtual const char* SLAPI text() const = 0;
	virtual const char* SLAPI value() const = 0;
	virtual const std::vector<ISLXmlNode*>& SLAPI getAllChilds() const = 0;
};

class ISLXmlReader{
public:
	virtual bool SLAPI loadXmlFile(const char* path) = 0;
	virtual const ISLXmlNode& SLAPI root() const = 0;
	virtual void SLAPI release() = 0;
};

extern "C" SL_DLL_API ISLXmlReader* SLAPI createXmlReader(void);
}
}
#endif