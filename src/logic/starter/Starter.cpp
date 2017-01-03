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
std::unordered_map<int32, Starter::Node> Starter::s_nodes;
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
		int32 type = nodes[i].getAttributeInt32("type");
		s_executes[type].min = nodes[i].getAttributeInt32("min");
		s_executes[type].max = nodes[i].getAttributeInt32("max");
		s_executes[type].type = type;
		SafeSprintf(s_executes[type].name, sizeof(s_executes[type].name), nodes[i].getAttributeString("name"));
		sl::api::ITimer* startNodeTimer = CREATE_POOL_OBJECT(StartNodeTimer, type);
		SLASSERT(startNodeTimer, "wtf");
		START_TIMER(startNodeTimer, 0, TIMER_BEAT_FOREVER, s_checkInterval);
	}
	return true;
}

bool Starter::destory(sl::api::IKernel * pKernel)
{
	DEL this;
	return true;
}

void Starter::startTimerInit(sl::api::IKernel * pKernel, int32 type){
	auto itor = s_executes.find(type);
	if (itor == s_executes.end()){
		SLASSERT(false, "wtf");
		return;
	}
	for (int32 i = 1; i < itor->second.min; i++){
		s_self->startNewNode(type, i, 1);
	}
	s_nodes[type].closeTick = 0;
	s_nodes[type].max = itor->second.min;
	s_nodes[type].online = false;
}

void Starter::startTimerOnTime(sl::api::IKernel * pKernel, int32 type){

}

void Starter::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){

}

void Starter::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){

}

void Starter::startNewNode(int32 nodeType, int32 nodeId, int32 slave){
	IArgs<2, 128> args;
	args << nodeType << nodeId;
	args.fix();
	s_harbor->send(NodeType::SLAVE, slave, NodeProtocol::MASTER_MSG_NEW_NODE, args.out());
}