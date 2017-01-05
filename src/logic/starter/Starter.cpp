#include "Starter.h"
#include "IHarbor.h"
#include "slxml_reader.h"
#include "slobjectpool.h"
#include "StartNodeTimer.h"
#include "slargs.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
using namespace sl;

Starter* Starter::s_self = nullptr;
sl::api::IKernel* Starter::s_kernel = nullptr;
IHarbor* Starter::s_harbor = nullptr;
std::unordered_map<int32, Starter::Execute> Starter::s_executes;
std::unordered_map<int32, Starter::NodeGroup> Starter::s_nodes;
int64 Starter::s_checkInterval = 0;
int64 Starter::s_deadTime = 0;

bool Starter::initialize(sl::api::IKernel * pKernel)
{
	s_self = this;
	s_kernel = pKernel;
	return true;
}

bool Starter::launched(sl::api::IKernel * pKernel)
{
	s_harbor =(IHarbor *)pKernel->findModule("Harbor");
	SLASSERT(s_harbor, "wtf");

	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "load core file %s failed", pKernel->getCoreFile());
		return false;
	}
	s_harbor->addNodeListener(this);

	const sl::xml::ISLXmlNode& starter = server_conf.root()["starter"][0];
	s_checkInterval = starter.getAttributeInt64("check");
	s_deadTime = starter.getAttributeInt64("dead");
	const sl::xml::ISLXmlNode& nodes = server_conf.root()["starter"][0]["node"];
	for (int32 i = 0; i < nodes.count(); i++){
		Execute info;
		info.type = nodes[i].getAttributeInt32("type");
		info.min = nodes[i].getAttributeInt32("min");
		info.max = nodes[i].getAttributeInt32("max");
		info.delay = nodes[i].getAttributeInt32("delay");
		info.timer = StartNodeTimer::create(info.type);
		s_executes[info.type] = info;
		SLASSERT(info.timer, "wtf");
		START_TIMER(info.timer, 0, TIMER_BEAT_FOREVER, s_checkInterval);
	}
	return true;
}

bool Starter::destory(sl::api::IKernel * pKernel)
{
	DEL this;
	return true;
}

void Starter::onNodeTimerStart(sl::api::IKernel * pKernel, int32 type, int64 tick){
	auto itor = s_executes.find(type);
	if (itor == s_executes.end()){
		SLASSERT(false, "wtf");
		return;
	}

	for (int32 i = 1; i <= itor->second.min; i++){
		s_self->startNode(pKernel, type, i);
	}
	s_nodes[type].max = itor->second.min;
}

void Starter::onNodeTimer(sl::api::IKernel * pKernel, int32 type, int64 tick){

}

void Starter::onNodeTimerEnd(sl::api::IKernel * pKernel, int32 type, int64 tick){
	if (s_executes.find(type) == s_executes.end())
		return;

	if (s_executes[type].timer == nullptr){
		SLASSERT(false, "execute no timer");
		return;
	}
	s_executes[type].timer->release();
	s_executes[type].timer = nullptr;
}

void Starter::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	if (nodeType == NodeType::SLAVE){
		return;
	}

	if (s_executes.find(nodeType) == s_executes.end()){
		return;
	}

	s_nodes[nodeType].nodes[nodeId].online = true;
	s_nodes[nodeType].nodes[nodeId].closeTick = 0;
}

void Starter::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	if (nodeType == NodeType::SLAVE){
		return;
	}

	s_nodes[nodeType].nodes[nodeId].online = false;
	s_nodes[nodeType].nodes[nodeId].closeTick = sl::getTimeMilliSecond();
}

void Starter::startNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId){
	IArgs<2, 128> args;
	args << nodeType << nodeId;
	args.fix();
	s_harbor->send(NodeType::SLAVE, 1, NodeProtocol::MASTER_MSG_START_NODE, args.out());
}