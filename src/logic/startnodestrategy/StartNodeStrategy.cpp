#include "StartNodeStrategy.h"
#include "slxml_reader.h"
#include "slargs.h"
#include "IStarter.h"
#include "GameDefine.h"
#include "NodeDefine.h"
bool StartNodeStrategy::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;

	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "load core file %s failed", pKernel->getCoreFile());
		return false;
	}
	const sl::xml::ISLXmlNode& scores = server_conf.root()["start_strategy"][0]["score"];
	for (int32 i = 0; i < scores.count(); i++){
		struct Score info;
		int32 type = scores[i].getAttributeInt32("type");
		info.forceNode = scores[i].getAttributeInt32("forcenode");
		info.value[BANDWIDTH] = scores[i].getAttributeInt32("bandwidth");
		info.value[OVERLOAD] = scores[i].getAttributeInt32("overload");
		_scores[type] = info;
	}

	return true;
}

bool StartNodeStrategy::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_starter, Starter);
	_starter->setStrategy(this);
	return true;
}

bool StartNodeStrategy::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void StartNodeStrategy::addSlave(const int32 nodeId){
	if (_slaves.find(nodeId) != _slaves.end()){
		SLASSERT(false, "wtf");
		return;
	}

	sl::SafeMemset(_slaves[nodeId].value, sizeof(_slaves[nodeId].value), 0, sizeof(_slaves[nodeId].value));
}

int32 StartNodeStrategy::chooseNode(const int32 nodeType, const int32 nodeId){
	if (_slaves.empty())
		return game::NODE_INVALID_ID;

	if (nodeType == NodeType::GATE)
		return choose(_kernel, nodeType, BANDWIDTH, OVERLOAD);
	else
		return choose(_kernel, nodeType, OVERLOAD, BANDWIDTH);
}

int32 StartNodeStrategy::choose(sl::api::IKernel* pKernel, int32 nodeType, int32 first, int32 second){
	SLASSERT(first >= 0 && first < DATA_COUNT && second >= 0 && second < DATA_COUNT, "wtf");
	int32 node = game::NODE_INVALID_ID;
	int32 value[DATA_COUNT];
	sl::SafeMemset(value, sizeof(value), -1, sizeof(value));

	if (_scores[nodeType].forceNode == 0){
		for (auto itor = _slaves.begin(); itor != _slaves.end(); ++itor){
			if (value[first] == -1 || value[first] > itor->second.value[first] ||
				(value[first] == itor->second.value[first] && value[second] > itor->second.value[second])){
				node = itor->first;
				value[first] = itor->second.value[first];
				value[second] = itor->second.value[second];
			}
		}
	}
	else{
		node = _scores[nodeType].forceNode;
	}

	if (node != game::NODE_INVALID_ID){
		for (int32 i = 0; i < DATA_COUNT; i++){
			_slaves[node].value[i] += _scores[nodeType].value[i];
		}
	}

	return node;
}