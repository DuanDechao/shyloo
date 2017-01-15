#include "IdMgr.h"
#include "sltools.h"
#include <string>
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "slxml_reader.h"
#include "slargs.h"
#include "sltime.h"

#define SEQUENCE_MASK 0x001F
#define TIMETICK_MASK 0x0001FFFFFFFFFFFF
#define AREA_MASK	  0x03FF
#define AREA_BITS	  10
#define SEQUENCE_BITS 5
#define GIVE_NUM	  20
#define GIVE_SIZE	  500
#define ASK_TIME_INTERVAL 500

IHarbor* IdMgr::s_harbor = nullptr;
IdMgr*	IdMgr::s_self = nullptr;
int32 IdMgr::s_svrNodeType = -2;
int32 IdMgr::s_areaId = 0;
int32 IdMgr::s_poolSize = 10000;
std::vector<uint64> IdMgr::s_idPool;
bool IdMgr::s_bIsMultiProcess = false;

bool IdMgr::initialize(sl::api::IKernel * pKernel){
	s_self = this;

	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "can't load core file %s", pKernel->getCoreFile());
		return false;
	}
	const sl::ISLXmlNode& idConf = server_conf.root()["id"][0];
	s_bIsMultiProcess = idConf.getAttributeBoolean("multiProc");
	if (s_bIsMultiProcess){
		s_svrNodeType = idConf.getAttributeInt32("server");
		s_areaId = idConf.getAttributeInt32("area");
		s_poolSize = idConf.getAttributeInt32("poolSize");
	}
	return true;
}

bool IdMgr::launched(sl::api::IKernel * pKernel){
	s_harbor = (IHarbor*)pKernel->findModule("Harbor");
	SLASSERT(s_harbor, "not find module harbor");
	if (s_bIsMultiProcess){
		s_harbor->rgsNodeMessageHandler(NodeProtocol::ASK_FOR_ALLOC_ID_AREA, IdMgr::askIds);
		s_harbor->rgsNodeMessageHandler(NodeProtocol::GIVE_ID_AREA, IdMgr::giveIds);
		START_TIMER(s_self, 20000, TIMER_BEAT_FOREVER, ASK_TIME_INTERVAL);
	}
	return true;
}
bool IdMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void IdMgr::onTime(sl::api::IKernel* pKernel, int64 timetick){
	if ((int32)s_idPool.size() < s_poolSize){
		IArgs<1, 32> args;
		args << 0;
		args.fix();
		s_harbor->send(s_svrNodeType, 1, NodeProtocol::ASK_FOR_ALLOC_ID_AREA, args.out());
	}
}

uint64 IdMgr::allocID(){
	if (!s_bIsMultiProcess){
		return s_self->generateId();
	}

	SLASSERT(s_idPool.size() > 0, "wtf");
	uint64 newId = *(s_idPool.rbegin());
	s_idPool.pop_back();
	return newId;
}

void IdMgr::askIds(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	IArgs<GIVE_NUM, GIVE_SIZE> inArgs;
	for (int32 i = 0; i < GIVE_NUM; i++){
		inArgs << s_self->generateId();
	}
	inArgs.fix();
	s_harbor->send(nodeType, nodeId, NodeProtocol::GIVE_ID_AREA, inArgs.out());
}

void IdMgr::giveIds(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	for (int32 i = 0; i < args.getCount(); i++){
		s_idPool.push_back(args.getInt64(i));
	}
}

uint64 IdMgr::generateId(){
	static uint64 lastTimeTick = sl::getTimeMilliSecond();
	static uint16 sequence = 0;

	uint64 nowTimeTick = sl::getTimeMilliSecond();
	if (lastTimeTick == nowTimeTick){
		sequence = (sequence + 1) & SEQUENCE_MASK;
		if (sequence == 0){
			while (lastTimeTick == nowTimeTick)
				nowTimeTick = sl::getTimeMilliSecond();
		}
	}
	else{
		sequence = 0;
	}
	lastTimeTick = nowTimeTick;
	uint64 newid = ((nowTimeTick & TIMETICK_MASK) << (AREA_BITS + SEQUENCE_MASK)) | ((s_areaId & AREA_MASK) << SEQUENCE_BITS) | (sequence & SEQUENCE_MASK);
	return newid;
}

