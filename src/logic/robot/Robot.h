#ifndef __SL_LOGIC_ROBOT_H__
#define __SL_LOGIC_ROBOT_H__
#include "slikernel.h"
#include "slimodule.h"
#include "IClient.h"
#include "slbinary_stream.h"
#include "slsingleton.h"
#include <unordered_map>
using namespace sl;

class Robot : public sl::api::IModule, public IClientListener{
	struct RobotInfo{
		string name;
		int64 ticket;
	};

	typedef void(Robot::*svr_args_cb)(sl::api::IKernel* pKernel, const OBStream& args);

public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onServerConnected(sl::api::IKernel* pKernel);
	virtual int32 onServerMsg(sl::api::IKernel* pKernel, const void* context, const int32 size);
	virtual void onServerDisConnected(sl::api::IKernel* pKernel);

	virtual void rgsSvrMessageHandler(int32 messageId, svr_args_cb handler);

	void onServerGiveGateAddressAck(sl::api::IKernel* pKernel, const OBStream& args);
	void onServerLoginAck(sl::api::IKernel* pKernel, const OBStream& args);
	void onServerCreateRoleAck(sl::api::IKernel* pKernel, const OBStream& args);
	void onServerSelectRoleAck(sl::api::IKernel* pKernel, const OBStream& args);
	void onServerAttribSync(sl::api::IKernel* pKernel, const OBStream& args);

	void test(sl::api::IKernel* pKernel);

private:
	void sendToSvr(sl::api::IKernel* pKernel, const int32 msgId, const OBStream& buf);

private:
	sl::api::IKernel*	_kernel;
	Robot*				_self;
	IClient*			_client;

	string				_svrIp;
	int32				_svrPort;
	RobotInfo			_robot;
	std::unordered_map<int32, svr_args_cb> _svrProtos;
};
#endif