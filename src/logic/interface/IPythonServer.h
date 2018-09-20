#ifndef _SL_INTERFACE_PYTHONSERVER_H__
#define _SL_INTERFACE_PYTHONSERVER_H__
#include "slimodule.h"
#include "slbinary_stream.h"
#include "slxml_reader.h"
#include "IObjectDef.h"
class IObject;
class IPythonServer : public sl::api::IModule{
public:
	virtual ~IPythonServer() {}
	virtual IDataType* createPyDataTypeFromXml(const char* typeName, const sl::ISLXmlNode* node) = 0; 
	virtual IDataType* createPyDataType(const char* typeName) = 0;
};

#endif
