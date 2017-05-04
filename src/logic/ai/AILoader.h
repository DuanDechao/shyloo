#ifndef __SL_FRAMEWORK_AI_LOADER_H__
#define __SL_FRAMEWORK_AI_LOADER_H__
#include "slikernel.h"
#include "slxml_reader.h"
#include <unordered_map>
#include <string>

class AIMachine;
class AINode;
class AICondition;
class AILoader{
public:
	typedef AINode* (*LoadNodeFuncType)(sl::api::IKernel* pKernel, const char* nodeName, AICondition* condition, const sl::xml::ISLXmlNode* node);
	typedef std::unordered_map<std::string, LoadNodeFuncType> LoadNodeFuncMap;
	
	static void initialize();
	static AIMachine* load(sl::api::IKernel* pKernel, const char* file, int32& interval);

private:
	static AINode* loadNode(sl::api::IKernel* pKernel, const sl::xml::ISLXmlNode* node);

	static AINode* loadSelect(sl::api::IKernel* pKernel, const char* nodeName, AICondition* condition, const sl::xml::ISLXmlNode* node);
	static AINode* loadSequence(sl::api::IKernel* pKernel, const char* nodeName, AICondition* condition, const sl::xml::ISLXmlNode* node);

private:
	static LoadNodeFuncMap s_nodeLoadFuncs;
};
#endif