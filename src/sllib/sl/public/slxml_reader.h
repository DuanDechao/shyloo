#ifndef SL_PUBLIC_XML_READER_H
#define SL_PUBLIC_XML_READER_H
#include "slxml.h"
#include "../sllib/api/slxml.h"
namespace sl
{
using namespace xml;
class XmlReader{
public:
	XmlReader()
		:m_xmlReader(createXmlReader())
	{}

	~XmlReader()
	{
		if (m_xmlReader)
			m_xmlReader->release();
		m_xmlReader = nullptr;
	}

	bool loadXml(const char* path)
	{
		return m_xmlReader->loadXmlFile(path);
	}

	const ISLXmlNode& root()
	{
		return m_xmlReader->root();
	}
private:
	ISLXmlReader* m_xmlReader;
};
}
#endif