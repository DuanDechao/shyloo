#include "Robot.h"
#include "slxml_reader.h"
#include "slbinary_stream.h"
#include "ProtocolID.pb.h"
#include "Protocol.pb.h"

bool Robot::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;

	return true;
}

bool Robot::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_client, Client);
	
	//_self->rgsSvrMessageHandler(AgentProtocol::CLIENT_MSG_LOGIN_REQ, &Robot::onClientLoginReq);
	//_self->rgsSvrMessageHandler(AgentProtocol::CLIENT_MSG_SELECT_ROLE_REQ, &Robot::onClientSelectRoleReq);
	_self->rgsSvrMessageHandler(ServerMsgID::SERVER_MSG_LOGIN_RSP, &Robot::onServerLoginAck);
	
	_client->setListener(this);

	char path[256] = { 0 };
	SafeSprintf(path, sizeof(path), "%s/robot.xml", pKernel->getEnvirPath());
	sl::XmlReader conf;
	if (!conf.loadXml(path)){
		SLASSERT(false, "can not load file %s", pKernel->getEnvirPath());
		return false;
	}

	_robotCount = conf.root().getAttributeInt32("count");
	_svrIp = conf.root()["svr"][0].getAttributeString("ip");
	_svrPort = conf.root()["svr"][0].getAttributeInt32("port");

	START_TIMER(_self, 0, 1, 5000);
	
	return true;
}

bool Robot::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Robot::onAgentOpen(sl::api::IKernel* pKernel, const int64 id){
	SLASSERT(_robots.find(id) == _robots.end(), "duplicate agent id£º%d", id);
	_robots[id] = { id, "ddc"};

	IBStream<64> args;
	args << _robots[id].name.c_str();
	sendToSvr(pKernel, id, ClientMsgID::CLIENT_MSG_LOGIN_REQ, args.out());
}

void Robot::onAgentClose(sl::api::IKernel* pKernel, const int64 id){
	SLASSERT(_robots.find(id) != _robots.end(), "where is agent %d", id);
	_robots.erase(id);
}

int32 Robot::onAgentRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size){
	if (size < sizeof(int32)* 2){
		return 0;
	}

	int32 len = *((int32*)((const char*)context + sizeof(int32)));
	if (size < len){
		return 0;
	}

	int32 msgId = *((int32*)context);
	if (_svrProtos.find(msgId) != _svrProtos.end()){
		OBStream buf((const char*)context + sizeof(int32)* 2, len);
		(this->*_svrProtos[msgId])(pKernel, id, buf);
	}
	else{
		//SLASSERT(false, "can not find message id[%d]", msgId);
	}
	return len;
}

void Robot::rgsSvrMessageHandler(int32 messageId, svr_args_cb handler){
	SLASSERT(_svrProtos.find(messageId) == _svrProtos.end(), "duplicate agent msg %d", messageId);
	
	_svrProtos[messageId] = handler;
}

void Robot::onTime(sl::api::IKernel* pKernel, int64 timetick){
	for (int32 i = 0; i < _robotCount; i++){
		_client->connect(_svrIp.c_str(), _svrPort);
	}
}

void Robot::sendToSvr(sl::api::IKernel* pKernel, const int64 id, const int32 msgId, const OBStream& buf){
	int32 header[2];
	header[0] = msgId;
	header[1] = buf.getSize() + sizeof(int32)* 2;

	_client->send(id, header, sizeof(header));
	_client->send(id, buf.getContext(), buf.getSize());
	ECHO_ERROR("send msg[%d] to svr", msgId);
}

void Robot::onServerLoginAck(sl::api::IKernel* pKernel, const int64 id, const OBStream& args){
	int32 errCode = protocol::ErrorCode::ERROR_NO_ERROR;
	if (!args.read(errCode) || errCode != protocol::ErrorCode::ERROR_NO_ERROR){
		SLASSERT(false, "login ack failed");
		return;
	}

	int32 roleCount = 0;
	if (!args.read(roleCount)){
		SLASSERT(false, "wtf");
		return;
	}

	if (roleCount < 3){
		static char names[3][64] = { "world", "wonder", "sky" };
		IBStream<256> ask;
		ask << names[roleCount] << (int8)1 << (int8)1;
		sendToSvr(pKernel, id, ClientMsgID::CLIENT_MSG_CREATE_ROLE_REQ, ask.out());
	}
	else{
		int64 actorId = 0;
		if (!args.read(actorId)){
			SLASSERT(false, "wtf");
			return;
		}

		IBStream<64> ask;
		ask << actorId;
		sendToSvr(pKernel, id, ClientMsgID::CLIENT_MSG_SELECT_ROLE_REQ, ask.out());
	}

}