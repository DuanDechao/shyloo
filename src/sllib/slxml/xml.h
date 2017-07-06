#ifndef SL_XML_H
#define SL_XML_H
#include "slxml.h"
#include "tinyxml.h"
#include <vector>
#include <map>
namespace sl{
namespace xml{

class CSLXMLNull : public ISLXmlNode{
	virtual int8 SLAPI getAttributeInt8(const char* name) const { SLASSERT(false, "this is null xml node"); return 0; }
	virtual int16 SLAPI getAttributeInt16(const char* name)const { SLASSERT(false, "this is null xml node"); return 0; }
	virtual int32 SLAPI getAttributeInt32(const char* name)const { SLASSERT(false, "this is null xml node"); return 0; }
	virtual int64 SLAPI getAttributeInt64(const char* name)const { SLASSERT(false, "this is null xml node"); return 0; }
	virtual const char* SLAPI getAttributeString(const char* name)const { SLASSERT(false, "this is null xml node"); return nullptr; }
	virtual float SLAPI getAttributeFloat(const char* name)const { SLASSERT(false, "this is null xml node"); return 0.0; }
	virtual bool SLAPI getAttributeBoolean(const char* name) const { SLASSERT(false, "this is null xml node"); return false; }
	virtual const ISLXmlNode& SLAPI operator[] (const char* nodeName)const { SLASSERT(false, "this is null xml node"); return *this; }
	virtual const ISLXmlNode& SLAPI operator[](const int32 i)const { SLASSERT(false, "this is null xml node"); return *this; }
	virtual const int32 SLAPI count(void) const { SLASSERT(false, "this is null xml node"); return 0; }
	virtual bool SLAPI hasAttribute(const char* name)const{ SLASSERT(false, "this is null xml node"); return false; }
	virtual bool SLAPI subNodeExist(const char* name) const { SLASSERT(false, "this is null xml node"); return false; }
	virtual const char* SLAPI text() const { SLASSERT(false, "this is null xml node"); return nullptr; }
	virtual const char* SLAPI value() const { SLASSERT(false, "this is null xml node"); return nullptr; }
	virtual const std::vector<ISLXmlNode*>& SLAPI getAllChilds() const { SLASSERT(false, "this is null xml node"); return _emptyNodes; }

private:
	std::vector<ISLXmlNode*> _emptyNodes;

};

class CSLXmlArray;
class CSLXmlNode : public ISLXmlNode{
public:
	struct AttrVal{
		std::string AttrStr;
		int64		AttrInt64;
		float		AttrFloat;
		bool		AttrBoolean;
	};

	CSLXmlNode(const TiXmlElement* poTiXmlNode);
	virtual ~CSLXmlNode();
	virtual int8 SLAPI getAttributeInt8(const char* name) const;
	virtual int16 SLAPI getAttributeInt16(const char* name) const;
	virtual int32 SLAPI getAttributeInt32(const char* name) const ;
	virtual int64 SLAPI getAttributeInt64(const char* name) const;
	virtual const char* SLAPI getAttributeString(const char* name) const;
	virtual float SLAPI getAttributeFloat(const char* name) const;
	virtual bool SLAPI getAttributeBoolean(const char* name) const;
	virtual const ISLXmlNode& SLAPI operator[] (const char* nodeName) const;
	virtual const ISLXmlNode& SLAPI operator[](const int32 i) const { SLASSERT(false, "this is xml node"); return m_xmlNull; }
	virtual const int32 SLAPI count(void) const { SLASSERT(false, "this is xml node"); return 0; }
	virtual bool SLAPI hasAttribute(const char* name) const {
		if (m_xmlAttrs.find(name) != m_xmlAttrs.end())
			return true;
		return false;
	}
	virtual const std::vector<ISLXmlNode*>& SLAPI getAllChilds() const { return m_allChilds; }

	virtual const char* SLAPI text() const { return m_text.c_str(); }
	virtual const char* SLAPI value() const { return m_value.c_str(); }
	virtual bool SLAPI subNodeExist(const char* name) const { return m_xmlChilds.find(name) != m_xmlChilds.end(); }
	void loadChildren(const TiXmlElement* element);
	void loadAttributes(const TiXmlElement* element);
	void loadText(const TiXmlElement* element);
	const AttrVal* findAttr(const char* name) const;
private:
	std::map<std::string, CSLXmlArray*> m_xmlChilds;
	std::vector<ISLXmlNode*> m_allChilds;
	std::map<std::string, AttrVal> m_xmlAttrs;
	std::string m_text;
	std::string m_value;
	CSLXMLNull	m_xmlNull;
	const TiXmlElement* m_poTiXmlNode;
};

class CSLXmlArray : public ISLXmlNode{
public:
	CSLXmlArray(const char* value) :m_value(value){}
	virtual	~CSLXmlArray(){
		for (int32 i = 0; i < (int32)m_elements.size(); i++){
			DEL m_elements[i];
		}
		m_elements.clear();
	}
	virtual int8 SLAPI getAttributeInt8(const char* name) const { SLASSERT(false, "this is xml array"); return 0; }
	virtual int16 SLAPI getAttributeInt16(const char* name) const { SLASSERT(false, "this is xml array"); return 0; }
	virtual int32 SLAPI getAttributeInt32(const char* name) const { SLASSERT(false, "this is xml array"); return 0; }
	virtual int64 SLAPI getAttributeInt64(const char* name) const { SLASSERT(false, "this is xml array"); return 0; }
	virtual const char* SLAPI getAttributeString(const char* name) const { SLASSERT(false, "this is xml array"); return nullptr; }
	virtual float SLAPI getAttributeFloat(const char* name) const { SLASSERT(false, "this is xml array"); return 0.0; }
	virtual bool SLAPI getAttributeBoolean(const char* name) const { SLASSERT(false, "this is xml array"); return false; }
	virtual bool SLAPI hasAttribute(const char* name)const{ SLASSERT(false, "this is xml array"); return false; }
	virtual bool SLAPI subNodeExist(const char* name) const { SLASSERT(false, "this is xml array"); return false; }
	virtual const ISLXmlNode& SLAPI operator[] (const char* nodeName)const { SLASSERT(false, "this is xml array"); return *this; }
	virtual const ISLXmlNode& SLAPI operator[](const int32 i) const{
		if (i < 0 || i >= (int32)m_elements.size()){
			SLASSERT(false, "invalid index");
			return m_xmlNull;
		}
		return *m_elements[i];
	}
	virtual const std::vector<ISLXmlNode*>& SLAPI getAllChilds() const { SLASSERT(false, "this is xml array"); return _emptyNodes; }

	virtual const int32 SLAPI count(void) const { return (int32)m_elements.size(); }
	virtual const char* SLAPI text() const { SLASSERT(false, "this is xml array"); return nullptr; }
	virtual const char* SLAPI value() const { return m_value.c_str(); }
	void addElement(CSLXmlNode* poXmlNode){
		if (poXmlNode)
			m_elements.push_back(poXmlNode);
	}
private:
	std::vector<CSLXmlNode*> m_elements;
	std::string m_value;
	CSLXMLNull	m_xmlNull;
	std::vector<ISLXmlNode*> _emptyNodes;
};


class CSLXmlReader : public ISLXmlReader{
public:
	CSLXmlReader();
	virtual ~CSLXmlReader();
	virtual bool SLAPI loadXmlFile(const char* path);
	virtual const ISLXmlNode& SLAPI root() const;
	virtual void SLAPI release();
private:
	CSLXmlNode* m_pRootNode;
};

}
}
#endif
