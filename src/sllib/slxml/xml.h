#ifndef SL_XML_H
#define SL_XML_H
#include "slxml.h"
#include "tinyxml.h"
#include <vector>
namespace sl
{
namespace xml
{
class CSLXmlNode : public ISLXmlNode
{
public:
	CSLXmlNode();
	virtual int8 SLAPI getAttributeInt8(const char* name);
	virtual int16 SLAPI getAttributeInt16(const char* name);
	virtual int32 SLAPI getAttributeInt32(const char* name);
	virtual int64 SLAPI getAttributeInt64(const char* name);
	virtual const char* SLAPI getAttributeString(const char* name);
	virtual float SLAPI getAttributeFloat(const char* name);
	virtual ISLXmlNode* operator[] (const char* nodeName);
	virtual ISLXmlNode* operator[] (const size_t i);

	void pushXmlNode(TiXmlNode* poXmlNode);
private:
	std::vector<TiXmlNode*> m_allXmlNode;
};

class CSLXmlBase : public ISLXmlBase
{
public:
	CSLXmlBase(TiXmlDocument* root);
	virtual ISLXmlNode* SLAPI root();
private:
	TiXmlDocument* m_pXmlDocument;
};
}
}
#endif