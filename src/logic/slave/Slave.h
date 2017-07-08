#ifndef SL_LOGIC_SLAVE_H
#define SL_LOGIC_SLAVE_H
#include "slikernel.h"
#include "slimodule.h"
#include <unordered_map>
#include "slsingleton.h"

#ifdef SL_OS_LINUX
#include <sys/wait.h>
#endif

#define MAX_CMD_LEN		256

class IHarbor;
class OArgs;
class Slave :public sl::api::IModule, public sl::SLHolder<Slave>{
public:
	
	struct EXECUTE_INFO{
		char name[64];
		char cmd[MAX_CMD_LEN];
	};

	class CheckNodeTimer;
	struct EXECUTE_NODE{
		EXECUTE_NODE() : timer(nullptr), process(0){}

		char cmd[MAX_CMD_LEN];
		CheckNodeTimer*	timer;
#ifdef SL_OS_WINDOWS
		int32 process;
#else
		pid_t process;
#endif
	};


	class CheckNodeTimer : public sl::api::ITimer{
	public:
		CheckNodeTimer(EXECUTE_NODE& node):_node(node){}
		virtual ~CheckNodeTimer(){}

		virtual void onStart(sl::api::IKernel* pKernel, int64 timetick) {}
		virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
		virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick) { DEL this; }
		virtual void onPause(sl::api::IKernel* pKernel, int64 timetick) {}
		virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

	private:
		EXECUTE_NODE& _node;
	};

	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void openNewNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void stopNodes(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);

private:
	void startNewNode(sl::api::IKernel* pKernel, const char* name, const char* cmd, const int32 nodeType, const int32 nodeId);
#ifdef SL_OS_WINDOWS
	int32 startNode(sl::api::IKernel* pKernel, const char* cmd);
#else
	pid_t startNode(sl::api::IKernel* pKernel, const char* cmd);
#endif
	

private:
	IHarbor* _harbor;

	int32	_startPort;
	int32	_endPort;
	int32	_startOutPort;
	int32   _endOutPort;
	int32   _balancePort;
	std::unordered_map<int64, EXECUTE_NODE> _cmds;
	std::unordered_map<int32, EXECUTE_INFO> _executes;
};

#endif
