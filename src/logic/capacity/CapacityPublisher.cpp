#include "CapacityPublisher.h"
#include "NodeDefine.h"
#include "slxml_reader.h"
#include "IHarbor.h"
bool CapacityPublisher::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool CapacityPublisher::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() != NodeType::MASTER){
	}
	return true;
}

bool CapacityPublisher::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}
