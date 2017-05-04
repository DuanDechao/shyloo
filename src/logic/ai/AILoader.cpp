#include "AILoader.h"
#include "AIMachine.h"
#include "AICondition.h"
#include "composites/Sequence.h"
#include "composites/Selector.h"

AILoader::LoadNodeFuncMap AILoader::s_nodeLoadFuncs;

void AILoader::initialize(){
	s_nodeLoadFuncs["select"] = loadSelect;
	s_nodeLoadFuncs["sequence"] = loadSequence;
}

AIMachine* AILoader::load(sl::api::IKernel* pKernel, const char* file, int32& interval){
	sl::XmlReader aiConf;
	if (!aiConf.loadXml(file)){
		SLASSERT(false, "load ai file %s failed", file);
		return false;
	}
	const sl::xml::ISLXmlNode& root = aiConf.root();
	interval = root.getAttributeInt32("interval");
	
	const std::vector<sl::xml::ISLXmlNode*> childs = root.getAllChilds();
	if (childs.size() != 1){
		SLASSERT(false, "config error");
		return nullptr;
	}

	return NEW AIMachine(loadNode(pKernel, childs[0]));
}

AINode* AILoader::loadNode(sl::api::IKernel* pKernel, const sl::xml::ISLXmlNode* node){
	const char* name = node->getAttributeString("name");
	AICondition* condition = nullptr;;
	if (node->hasAttribute("condition")){
		condition = NEW AICondition(node->getAttributeString("condition"));
	}

	auto itor = s_nodeLoadFuncs.find(node->value());
	if (itor == s_nodeLoadFuncs.end()){
		SLASSERT(false, "wtf");
		return nullptr;
	}
	else{
		return itor->second(pKernel, name, condition, node);
	}

	return nullptr;
}

AINode* AILoader::loadSelect(sl::api::IKernel* pKernel, const char* nodeName, AICondition* condition, const sl::xml::ISLXmlNode* node){
	Selector* selectNode = NEW Selector(condition);
	const std::vector<sl::xml::ISLXmlNode*> childs = node->getAllChilds();
	for (int32 i = 0; i < (int32)childs.size(); i++){
		selectNode->addChild(loadNode(pKernel, childs[i]));
	}
	return selectNode;
}

AINode* AILoader::loadSequence(sl::api::IKernel* pKernel, const char* nodeName, AICondition* condition, const sl::xml::ISLXmlNode* node){
	Sequence* seqNode = NEW Sequence(condition);
	const std::vector<sl::xml::ISLXmlNode*> childs = node->getAllChilds();
	for (int32 i = 0; i < (int32)childs.size(); i++){
		seqNode->addChild(loadNode(pKernel, childs[i]));
	}
	return seqNode;
}