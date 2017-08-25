#include "Starter.h"
#include "IHarbor.h"
#include "slxml_reader.h"
#include "StartNodeTimer.h"
#include "slargs.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "sltime.h"
#include "IEventEngine.h"
#include "EventID.h"
#include "ICapacity.h"
#define STARTED_DELAY_TIME 15000
bool Starter::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	_strategy = nullptr;
	_status = STATE::STAT_NOT_START;

	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "load core file %s failed", pKernel->getCoreFile());
		return false;
	}
	const sl::xml::ISLXmlNode& starter = server_conf.root()["starter"][0];
	_checkInterval = starter.getAttributeInt64("check");
	_deadTime = starter.getAttributeInt64("dead");
	const sl::xml::ISLXmlNode& nodes = server_conf.root()["starter"][0]["node"];
	for (int32 i = 0; i < nodes.count(); i++){
		Execute info;
		info.type = nodes[i].getAttributeInt32("type");
		info.min = nodes[i].getAttributeInt32("min");
		info.max = nodes[i].getAttributeInt32("max");
		info.delay = nodes[i].getAttributeInt32("delay");
		info.rate = nodes[i].getAttributeInt32("rate");
		info.timer = StartNodeTimer::create(info.type);
		_executes[info.type] = info;
		SLASSERT(info.timer, "wtf");
	}

	return true;
}

bool Starter::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() == NodeType::MASTER){
		_harbor->addNodeListener(this);

		FIND_MODULE(_eventEngine, EventEngine);
		FIND_MODULE(_capacitySubscriber, CapacitySubscriber);

		RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_PRE_SHUTDOWN, Starter::preShutDown);
	}
	
	return true;
}

bool Starter::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Starter::onTime(sl::api::IKernel* pKernel, int64 timetick){
	IArgs<1, 256> args;
	args.fix();

	_harbor->broadcast(NodeProtocol::MASTER_MSG_SERVER_STARTED, args.out());
}

void Starter::onNodeTimerStart(sl::api::IKernel * pKernel, int32 type, int64 tick){
	auto itor = _executes.find(type);
	if (itor == _executes.end()){
		SLASSERT(false, "wtf");
		return;
	}

	for (int32 i = 1; i <= itor->second.min; i++){
		_self->startNode(pKernel, type, i);
	}
	_nodes[type].max = itor->second.min;
}

void Starter::onNodeTimer(sl::api::IKernel * pKernel, int32 type, int64 tick){
	auto itor = _nodes.find(type);
	SLASSERT(itor != _nodes.end(), "wtf");
	if (itor != _nodes.end()){
		if (_executes[itor->first].max > itor->second.max && _capacitySubscriber->checkOverLoad(itor->first, _executes[itor->first].rate * itor->second.max)){
			startNode(pKernel, itor->first, itor->second.max + 1);
			++itor->second.max;
		}
	}
}

void Starter::onNodeTimerEnd(sl::api::IKernel * pKernel, int32 type, int64 tick){
	if (_executes.find(type) == _executes.end())
		return;

	if (_executes[type].timer == nullptr){
		SLASSERT(false, "execute no timer");
		return;
	}
	_executes[type].timer->release();
	_executes[type].timer = nullptr;
}

void Starter::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	if (nodeType == NodeType::SLAVE){
		if (_strategy)
			_strategy->addSlave(nodeId);

		if (_status == STATE::STAT_NOT_START){
			startServer(pKernel);
			_status = STATE::STAT_STARTING;
		}
		return;
	}

	if (_executes.find(nodeType) == _executes.end()){
		return;
	}

	_nodes[nodeType].nodes[nodeId].online = true;
	_nodes[nodeType].nodes[nodeId].closeTick = 0;

	if (testStarted(pKernel)){
		_status = STATE::STAT_STARTED;
		START_TIMER(_self, 0, 1, STARTED_DELAY_TIME);
	}
}

void Starter::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	if (nodeType == NodeType::SLAVE){
		return;
	}

	_nodes[nodeType].nodes[nodeId].online = false;
	_nodes[nodeType].nodes[nodeId].closeTick = sl::getTimeMilliSecond();
}

void Starter::startNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId){
	if (!_strategy){
		SLASSERT(false, "wtf");
		return;
	}

	int32 slave = _strategy->chooseNode(nodeType, nodeId);
	if (slave != game::NODE_INVALID_ID){
		IArgs<2, 128> args;
		args << nodeType << nodeId;
		args.fix();
		_harbor->send(NodeType::SLAVE, slave, NodeProtocol::MASTER_MSG_START_NODE, args.out());
		TRACE_LOG("start new Node %d %d", nodeType, nodeId);
	}
}


void Starter::startServer(sl::api::IKernel* pKernel){
	for (auto itor = _executes.begin(); itor != _executes.end(); ++itor){
		SLASSERT(itor->second.timer, "wtf");
		START_TIMER(itor->second.timer, itor->second.delay, TIMER_BEAT_FOREVER, _checkInterval);
	}
}

void Starter::preShutDown(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::PreShutDown), "wtf");
	std::vector<StartNodeTimer* > timers;
	for (auto itor = _executes.begin(); itor != _executes.end(); ++itor){
		if (itor->second.timer)
			timers.push_back(itor->second.timer);
	}

	for (auto* timer: timers){
		pKernel->killTimer(timer);
	}

	IArgs<1, 32> args;
	args.fix();
	_harbor->broadcast(NodeType::SLAVE, NodeProtocol::MASTER_MSG_STOP_NODES, args.out());
}

bool Starter::testStarted(sl::api::IKernel* pKernel){
	for (auto itor = _nodes.begin(); itor != _nodes.end(); ++itor){
		for (int32 i = 1; i < itor->second.max; i++){
			if (!itor->second.nodes[i].online){
				return false;
			}
		}
	}
	return true;
}
