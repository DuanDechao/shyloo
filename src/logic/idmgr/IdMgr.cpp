#include "IdMgr.h"
#include "sltools.h"
#include <string>
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "slxml_reader.h"
#include "slargs.h"
#include "sltime.h"
#include "NodeDefine.h"
#include "IResMgr.h"
#define SEQUENCE_MASK 0x001F
#define TIMETICK_MASK 0x0001FFFFFFFFFFFF
#define AREA_MASK	  0x03FF
#define AREA_BITS	  10
#define SEQUENCE_BITS 5
#define GIVE_NUM	  100
#define GIVE_SIZE	  1000
#define ASK_TIME_INTERVAL 2000
bool IdMgr::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_bIsMultiProcess = true;
	if (_bIsMultiProcess){
		_areaId = SLMODULE(ResMgr)->getResValueInt32("hostID");
		_poolSize = SLMODULE(ResMgr)->getResValueInt32("ids/desiredSize");
	}
	return true;
}

bool IdMgr::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_bIsMultiProcess){
		if (_harbor->getNodeType() == NodeType::MASTER){
			RGS_NODE_HANDLER(_harbor, NodeProtocol::ASK_FOR_ALLOC_ID_AREA, IdMgr::askIds);
		}
		else{
			RGS_NODE_HANDLER(_harbor, NodeProtocol::GIVE_ID_AREA, IdMgr::giveIds);
			SLMODULE(Harbor)->addNodeListener(this);
			SLMODULE(Cluster)->addServerProcessHandler(this);
		}
	}
	return true;
}
bool IdMgr::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}
	
void IdMgr::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	if(nodeType == NodeType::MASTER){
		START_TIMER(_self, 1000, TIMER_BEAT_FOREVER, ASK_TIME_INTERVAL);
	}
}

void IdMgr::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
}

bool IdMgr::onServerReady(sl::api::IKernel* pKernel){
	return (int32)_idPool.size() >= _poolSize;
}

void IdMgr::onTime(sl::api::IKernel* pKernel, int64 timetick){
	if ((int32)_idPool.size() < _poolSize){
		sl::BStream<32> args;
		args << 0;
		_harbor->send(NodeType::MASTER, 1, NodeProtocol::ASK_FOR_ALLOC_ID_AREA, args.out());
	}
}

uint64 IdMgr::allocID(){
	if (!_bIsMultiProcess){
		return _self->generateLocalId();
	}

	SLASSERT(_idPool.size() > 0, "wtf");
	uint64 newId = *(_idPool.rbegin());
	_idPool.pop_back();
	return newId;
}

void IdMgr::askIds(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	sl::BStream<GIVE_SIZE> inArgs;
	for (int32 i = 0; i < GIVE_NUM; i++){
		inArgs << _self->generateLocalId();
	}
	_harbor->send(nodeType, nodeId, NodeProtocol::GIVE_ID_AREA, inArgs.out());
}

void IdMgr::giveIds(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	while(args.getSize() > 0){
		int64 id = 0;
		args >> id;
		_idPool.push_back(id);
	}
}

uint64 IdMgr::generateLocalId(){
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

