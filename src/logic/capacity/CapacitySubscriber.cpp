#include "CapacitySubscriber.h"
#include "slxml_reader.h"
#include "IHarbor.h"
#include "NodeDefine.h"

bool CapacitySubscriber::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool CapacitySubscriber::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() != NodeType::MASTER){
	}
	return true;
}

bool CapacitySubscriber::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

