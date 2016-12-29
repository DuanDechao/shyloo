#include "Starter.h"
#include "IHarbor.h"
#include "slxml_reader.h"
#include "slobjectpool.h"
#include "StartNodeTimer.h"
using namespace sl;

Starter* Starter::s_self = nullptr;
sl::api::IKernel* Starter::s_kernel = nullptr;
IHarbor* Starter::s_harbor = nullptr;
std::unordered_map<int32, Starter::Execute> Starter::s_executes;
int64 Starter::s_checkInterval = 0;
int64 Starter::s_deadTime = 0;

bool Starter::initialize(sl::api::IKernel * pKernel)
{
	s_self = this;
	s_kernel = pKernel;
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
	const sl::xml::ISLXmlNode& nodes = server_conf.root()["harbor"][0]["node"];
	for (int32 i = 0; i < nodes.count(); i++){
		if (!nodes[i].hasAttribute("hidden") || !nodes[i].getAttributeBoolean("hidden")){
			int32 type = nodes[i].getAttributeInt32("type");
			SafeSprintf(s_executes[type].name, sizeof(s_executes[type].name), nodes[i].getAttributeString("name"));
			sl::api::ITimer* startNodeTimer = CREATE_POOL_OBJECT(StartNodeTimer, this, type);
			SLASSERT(startNodeTimer, "wtf");
			START_TIMER(startNodeTimer, 0, s_deadTime / s_checkInterval, s_checkInterval);
		}
	}
}

bool Starter::destory(sl::api::IKernel * pKernel)
{
	DEL this;
}

void Starter::startTimerInit(sl::api::IKernel * pKernel, int32 type){

}

void Starter::startTimerOnTime(sl::api::IKernel * pKernel, int32 type){

}