#include "xml.h"
#include "slstring_utils.h"
namespace sl
{
namespace xml
{

CSLXmlNode::CSLXmlNode()
	:m_allXmlNode()
{}

ISLXmlBase* SLAPI loadXmlFile(const char* path)
{
	if (nullptr == path)
		return nullptr;

	TiXmlDocument doc(path);
	if (!doc.LoadFile())
		return nullptr;
	return new CSLXmlBase(&doc);
}

int8 CSLXmlNode::getAttributeInt8(const char* name)
{
	return CStringUtils::StringAsInt8(m_allXmlNode[0]->ToElement()->Attribute(name));
}
int16 CSLXmlNode::getAttributeInt16(const char* name)
{
	return CStringUtils::StringAsInt16(m_allXmlNode[0]->ToElement()->Attribute(name));
}

int32 CSLXmlNode::getAttributeInt32(const char* name)
{
	return CStringUtils::StringAsInt32(m_allXmlNode[0]->ToElement()->Attribute(name));
}

int64 CSLXmlNode::getAttributeInt64(const char* name)
{
	return CStringUtils::StringAsInt64(m_allXmlNode[0]->ToElement()->Attribute(name));
}

const char * CSLXmlNode::getAttributeString(const char* name)
{
	return m_allXmlNode[0]->ToElement()->Attribute(name);
}

float CSLXmlNode::getAttributeFloat(const char* name)
{
	return CStringUtils::StringAsFloat(m_allXmlNode[0]->ToElement()->Attribute(name));
}

ISLXmlNode* CSLXmlNode::operator[](const char* nodeName)
{
	if (nullptr == nodeName || m_allXmlNode.empty()){
		return nullptr;
	}
	
	TiXmlNode* pXmlNode = m_allXmlNode[0]->FirstChild(nodeName);
	if (nullptr == pXmlNode)
		return nullptr;

	CSLXmlNode* pNewSLXmlNode = new CSLXmlNode();
	if (nullptr == pNewSLXmlNode)
		return nullptr;

	while (pXmlNode){
		pNewSLXmlNode->pushXmlNode(pXmlNode);
		pXmlNode = pXmlNode->NextSibling(nodeName);
	}

	return pNewSLXmlNode;
}

ISLXmlNode* CSLXmlNode::operator[](const size_t i){
	if (m_allXmlNode.empty() || m_allXmlNode.size() <= i)
		return nullptr;

	CSLXmlNode* pNewSLXmlNode = new CSLXmlNode();
	if (nullptr == pNewSLXmlNode)
		return nullptr;

	pNewSLXmlNode->pushXmlNode(m_allXmlNode[i]);
	return pNewSLXmlNode;
}

void CSLXmlNode::pushXmlNode(TiXmlNode* poXmlNode)
{
	if (poXmlNode == nullptr)
		return;

	m_allXmlNode.push_back(poXmlNode);
}

CSLXmlBase::CSLXmlBase(TiXmlDocument* root)
	:m_pXmlDocument(root)
{}

ISLXmlNode* CSLXmlBase::root()
{
	TiXmlNode* xmlNode = m_pXmlDocument->FirstChild();
	if (nullptr == xmlNode)
		return nullptr;

	CSLXmlNode* pNewSLXmlNode = new CSLXmlNode();
	if (nullptr == pNewSLXmlNode)
		return nullptr;

	pNewSLXmlNode->pushXmlNode(xmlNode);
	return pNewSLXmlNode;
}

}
}