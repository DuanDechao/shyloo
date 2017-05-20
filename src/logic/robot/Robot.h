#ifndef __SL_LOGIC_ROBOT_H__
#define __SL_LOGIC_ROBOT_H__
#include "slikernel.h"
#include "slimodule.h"
#include "IClient.h"
#include "slbinary_stream.h"
#include "slsingleton.h"
#include <unordered_map>
using namespace sl;

class Robot : public sl::api::IModule, public IClientListener, public sl::api::ITimer{
	struct RobotInfo{
		int64 clientId;
		string name;
	};

	typedef void(Robot::*svr_args_cb)(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);

public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onAgentOpen(sl::api::IKernel* pKernel, const int64 id);
	virtual int32 onAgentRecv(sl::api::IKernel* pKernel, const int64 id, const void* context, const int32 size);
	virtual void onAgentClose(sl::api::IKernel* pKernel, const int64 id);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

	virtual void rgsSvrMessageHandler(int32 messageId, svr_args_cb handler);

	void onServerLoginAck(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);
	void onServerCreateRoleAck(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);
	void onServerSelectRoleAck(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);
	void onServerAttribSync(sl::api::IKernel* pKernel, const int64 id, const OBStream& args);

protected:
	void test(sl::api::IKernel* pKernel, const int64 id);

private:
	void sendToSvr(sl::api::IKernel* pKernel, const int64 id, const int32 msgId, const OBStream& buf);

private:
	sl::api::IKernel*	_kernel;
	Robot*				_self;
	IClient*			_client;

	int32				_robotCount;
	std::string			_svrIp;
	int32				_svrPort;
	
	std::unordered_map<int32, svr_args_cb> _svrProtos;
	std::unordered_map<int64, RobotInfo> _robots;
};
#endif