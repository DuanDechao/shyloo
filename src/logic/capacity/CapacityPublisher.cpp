#include "CapacityPublisher.h"
#include "NodeDefine.h"
#include "slxml_reader.h"
#include "slargs.h"
#include "GameDefine.h"
#include "NodeProtocol.h"

bool CapacityPublisher::initialize(sl::api::IKernel * pKernel){
	_lastLoad = 0;
	_currLoad = 0;
	_maxLoad = game::MAX_CAPACITY_LOAD;
	_self = this;
	return true;
}

bool CapacityPublisher::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	
	sl::XmlReader svrConf;
	if (!svrConf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "cant load core file");
		return false;
	}

	_harbor->addNodeListener(this);

	int32 broadInterval = svrConf.root()["loadbroad"][0].getAttributeInt32("interval");
	START_TIMER(_self, 0, -1, broadInterval);
	
	return true;
}

bool CapacityPublisher::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void CapacityPublisher::increaseLoad(int32 load){
	if (load <= 0)
		return;

	_lastLoad = _currLoad;
	_currLoad += load;
}

void CapacityPublisher::decreaseLoad(int32 load){
	if (load <= 0)
		return;
	 
	_lastLoad = _currLoad;
	_currLoad -= load;
}

void CapacityPublisher::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	IArgs<1, 64> args;
	args << (float)(_currLoad / _maxLoad);
	args.fix();
	_harbor->send(nodeType, nodeId, NodeProtocol::NODE_CAPACITY_LOAD_REPORT, args.out());
}

void CapacityPublisher::onTime(sl::api::IKernel* pKernel, int64 timetick){
	if (_lastLoad == _currLoad)
		return;

	_lastLoad = _currLoad;

	IArgs<1, 64> args;
	args << (float)_currLoad / _maxLoad;
	args.fix();
	_harbor->broadcast(NodeProtocol::NODE_CAPACITY_LOAD_REPORT, args.out());
}

