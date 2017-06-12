#include "IdMgr.h"
#include "sltools.h"
#include <string>
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "slxml_reader.h"
#include "slargs.h"
#include "sltime.h"
#include "NodeDefine.h"

#define SEQUENCE_MASK 0x001F
#define TIMETICK_MASK 0x0001FFFFFFFFFFFF
#define AREA_MASK	  0x03FF
#define AREA_BITS	  10
#define SEQUENCE_BITS 5
#define GIVE_NUM	  20
#define GIVE_SIZE	  500
#define ASK_TIME_INTERVAL 10000

bool IdMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;

	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "can't load core file %s", pKernel->getCoreFile());
		return false;
	}
	const sl::ISLXmlNode& idConf = server_conf.root()["id"][0];
	_bIsMultiProcess = idConf.getAttributeBoolean("multiProc");
	if (_bIsMultiProcess){
		_svrNodeType = idConf.getAttributeInt32("server");
		_areaId = idConf.getAttributeInt32("area");
		_poolSize = idConf.getAttributeInt32("poolSize");
	}
	return true;
}

bool IdMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_bIsMultiProcess){
		if (_harbor->getNodeType() == NodeType::MASTER){
			RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::ASK_FOR_ALLOC_ID_AREA, IdMgr::askIds);
		}
		else{
			RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::GIVE_ID_AREA, IdMgr::giveIds);
			START_TIMER(_self, 1000, TIMER_BEAT_FOREVER, ASK_TIME_INTERVAL);
		}
	}
	return true;
}
bool IdMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void IdMgr::onTime(sl::api::IKernel* pKernel, int64 timetick){
	if ((int32)_idPool.size() < _poolSize){
		IArgs<1, 32> args;
		args << 0;
		args.fix();
		_harbor->send(_svrNodeType, 1, NodeProtocol::ASK_FOR_ALLOC_ID_AREA, args.out());
	}
}

uint64 IdMgr::allocID(){
	if (!_bIsMultiProcess){
		return _self->generateId();
	}

	SLASSERT(_idPool.size() > 0, "wtf");
	uint64 newId = *(_idPool.rbegin());
	_idPool.pop_back();
	return newId;
}

void IdMgr::askIds(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	IArgs<GIVE_NUM, GIVE_SIZE> inArgs;
	for (int32 i = 0; i < GIVE_NUM; i++){
		inArgs << _self->generateId();
	}
	inArgs.fix();
	_harbor->send(nodeType, nodeId, NodeProtocol::GIVE_ID_AREA, inArgs.out());
}

void IdMgr::giveIds(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	for (int32 i = 0; i < args.getCount(); i++){
		_idPool.push_back(args.getInt64(i));
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
	uint64 newid = ((nowTimeTick & TIMETICK_MASK) << (AREA_BITS + SEQUENCE_MASK)) | ((_areaId & AREA_MASK) << SEQUENCE_BITS) | (sequence & SEQUENCE_MASK);
	return newid;
}

