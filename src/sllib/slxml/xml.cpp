#define SL_DLL_EXPORT
#include "xml.h"
#include "slstring_utils.h"
namespace sl{
namespace xml{

CSLXmlNode::CSLXmlNode(const TiXmlElement* poTiXmlNode){
	m_value = poTiXmlNode->Value();
	m_poTiXmlNode = poTiXmlNode;
	loadChildren(poTiXmlNode);
	loadAttributes(poTiXmlNode);
	loadText(poTiXmlNode);
}

CSLXmlNode::~CSLXmlNode(){
	std::map<std::string, CSLXmlArray*>::iterator iter = m_xmlChilds.begin();
	for (; iter != m_xmlChilds.end(); ++iter){
		DEL iter->second;
	}
	m_xmlChilds.clear();
	m_xmlAttrs.clear();
	m_allChilds.clear();
}

int8 CSLXmlNode::getAttributeInt8(const char* name) const{
	const AttrVal* val = findAttr(name);
	return val ? (int8)val->AttrInt64 : 0;
}

int16 CSLXmlNode::getAttributeInt16(const char* name) const{
	const AttrVal* val = findAttr(name);
	return val ? (int16)val->AttrInt64 : 0;
}

int32 CSLXmlNode::getAttributeInt32(const char* name) const{
	const AttrVal* val = findAttr(name);
	return val ? (int32)val->AttrInt64 : 0;
}

int64 CSLXmlNode::getAttributeInt64(const char* name) const{
	const AttrVal* val = findAttr(name);
	return val ? val->AttrInt64 : 0;
}

const char * CSLXmlNode::getAttributeString(const char* name) const{
	const AttrVal* val = findAttr(name);
	return val ? val->AttrStr.c_str() : nullptr;
}

float CSLXmlNode::getAttributeFloat(const char* name) const{
	const AttrVal* val = findAttr(name);
	return val ? val->AttrFloat : (float)0.0;
}

bool CSLXmlNode::getAttributeBoolean(const char* name) const{
	const AttrVal* val = findAttr(name);
	return val ? val->AttrBoolean : false;
}

const ISLXmlNode& CSLXmlNode::operator[](const char* nodeName) const{
	if (nullptr == nodeName){
		SLASSERT(false, "invalid params");
		return m_xmlNull;
	}
	auto itor = m_xmlChilds.find(nodeName);
	if (itor == m_xmlChilds.end()){
		SLASSERT(false, "where is child node %s", nodeName);
		return m_xmlNull;
	}
	return *itor->second;
}

void CSLXmlNode::loadChildren(const TiXmlElement* element){
	for (auto * node = element->FirstChildElement(); node; node = node->NextSiblingElement()){
		if (m_xmlChilds.find(node->Value()) == m_xmlChilds.end()){
			m_xmlChilds[node->Value()] = NEW CSLXmlArray(node->Value());
		}
		CSLXmlNode* child = NEW CSLXmlNode(node);
		m_xmlChilds[node->Value()]->addElement(child);
		m_allChilds.push_back(child);
	}
}

void CSLXmlNode::loadAttributes(const TiXmlElement* element){
	for (auto* attr = element->FirstAttribute(); attr; attr = attr->Next()){
		const char* name = attr->Name();
		const char* value = attr->Value();

		AttrVal attrVal;
		attrVal.AttrStr = value;
		attrVal.AttrInt64 = CStringUtils::StringAsInt64(value);
		attrVal.AttrFloat = CStringUtils::StringAsFloat(value);
		attrVal.AttrBoolean = CStringUtils::StringAsBoolean(value);
		m_xmlAttrs[name] = attrVal;
	}
}

void CSLXmlNode::loadText(const TiXmlElement* element){
	if (element->GetText())
		m_text = element->GetText();
}

const CSLXmlNode::AttrVal* CSLXmlNode::findAttr(const char* name) const{
	if (name == nullptr)
		return nullptr;

	auto iter = m_xmlAttrs.find(name);
	if (iter != m_xmlAttrs.end())
		return &iter->second;

	SLASSERT(false, "where is attr %s", name);
	return nullptr;
}

CSLXmlReader::CSLXmlReader()
	:m_pRootNode(nullptr)
{}

CSLXmlReader::~CSLXmlReader(){
	if (m_pRootNode)
		DEL m_pRootNode;
	m_pRootNode = nullptr;
}

bool CSLXmlReader::loadXmlFile(const char* path){
	if (nullptr == path)
		return false;

	TiXmlDocument doc(path);
	if (!doc.LoadFile())
		return false;

	TiXmlElement* xmlNode = doc.RootElement();
   //	const char* dn = xmlNode->Value();
	if (nullptr == xmlNode)
		return false;
	m_pRootNode = NEW CSLXmlNode(xmlNode);
	if (nullptr == m_pRootNode)
		return false;

	return true;
}

const ISLXmlNode& CSLXmlReader::root() const{
	SLASSERT(m_pRootNode, "where is root node");
	return *m_pRootNode;
}

void CSLXmlReader::release(){
	DEL this;
}

extern "C" SL_DLL_API ISLXmlReader* SLAPI createXmlReader(){
	return NEW CSLXmlReader;
}

}
}
