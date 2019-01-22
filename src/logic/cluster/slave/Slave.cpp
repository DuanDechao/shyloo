#include "Slave.h"
#include "sltools.h"
#include <string>
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "slxml_reader.h"
#include "slargs.h"
#include "sltools.h"
#include "IDebugHelper.h"
#include "../machine/Machine.h"

#ifdef SL_OS_LINUX
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#endif

#define NODE_CHECK_INTERVAL			1000

#ifdef SL_OS_LINUX
#define MAX_CMD_ARGS_COUNT			256
#endif


void Slave::CheckNodeTimer::onTime(sl::api::IKernel* pKernel, int64 timetick){
#ifdef SL_OS_LINUX
	if (_node.process > 0){
		int32 status = 0;
		pid_t ret = waitpid(_node.process, &status, WNOHANG);
		if (ret != 0){
			if (ret > 0){
				ECHO_ERROR("process [%s] is lost, try restart", _node.cmd);
				_node.process = Slave::getInstance()->startNode(pKernel, _node.cmd);
			}
			else{
				ret = kill(_node.process, 0);
				if(ret != 0){
					ECHO_ERROR("process [%s] is lost, try restart", _node.cmd);
					_node.process = Slave::getInstance()->startNode(pKernel, _node.cmd);
				}
			}
		}
	}
	else{
		TRACE_LOG("process [%s] is not running, try again", _node.cmd);
		_node.process = Slave::getInstance()->startNode(pKernel, _node.cmd);
	}
#endif
}

bool Slave::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Slave::launched(sl::api::IKernel * pKernel){
	if(SLMODULE(Harbor)->getNodeType() != NodeType::SLAVE)
		return true;

	RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::MASTER_MSG_START_NODE, Slave::openNewNode);
	RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::MASTER_MSG_STOP_NODES, Slave::stopNodes);
	RGS_NODE_HANDLER(SLMODULE(Harbor), NodeProtocol::CLUSTER_MSG_REGISTER_NODE_TO_SLAVE, Slave::clusterRegisterNode);

	START_TIMER(_self, 0, -1, 1000);

	return true;
}
bool Slave::destory(sl::api::IKernel * pKernel){
	for (auto itor = _cmds.begin(); itor != _cmds.end(); ++itor){
		if (itor->second.timer){
			pKernel->killTimer(itor->second.timer);
			itor->second.timer = nullptr;
		}
	}

	DEL this;
	return true;
}

void Slave::openNewNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args){
	int32 newNodeType = 0;
	args >> newNodeType;
	int32 newNodeId = 0;
	args >> newNodeId;
	TRACE_LOG("open new Node [%d:%d]", newNodeType, newNodeId);

	char cmd[MAX_CMD_LEN] = {0};
	const char* newNodeName = SLMODULE(Harbor)->getNodeName(newNodeType);
	SafeSprintf(cmd, sizeof(cmd), "--name=%s", newNodeName);
	if(newNodeId > 0){
		SafeSprintf(cmd, sizeof(cmd), "--name=%s --node_id=%d", newNodeName, newNodeId);
	}

	int64 node = (((int64)newNodeType) << 32) | newNodeId;
	auto iter = _cmds.find(node);
	if (iter == _cmds.end())
		startNode(pKernel, cmd);
}

void Slave::stopNodes(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args){
	for (auto itor = _cmds.begin(); itor != _cmds.end(); ++itor){
		if (itor->second.timer){
			pKernel->killTimer(itor->second.timer);
			itor->second.timer = nullptr;
		}
	}
}

void Slave::clusterRegisterNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const sl::OBStream& args){
	int64 nodePid = 0;
	args >> nodePid;
	printf("register Node %d:%d %d\n", nodeType, nodeId, nodePid);
	
	const char* nodeName = SLMODULE(Harbor)->getNodeName(nodeType);
	int64 node = (((int64)nodeType) << 32) | nodeId;
	SafeSprintf(_cmds[node].cmd, sizeof(_cmds[node].cmd), "--name=%s --node_id=%d", nodeName, nodeId);
	_cmds[node].process = (pid_t)nodePid;

#ifdef SL_OS_LINUX
	_cmds[node].timer = NEW CheckNodeTimer(_cmds[node]);
	START_TIMER(_cmds[node].timer, 0, TIMER_BEAT_FOREVER, NODE_CHECK_INTERVAL);
#endif
}

#ifdef SL_OS_WINDOWS
int32 Slave::startNode(sl::api::IKernel* pKernel, const char* cmd){
#else
pid_t Slave::startNode(sl::api::IKernel* pKernel, const char* cmd){
#endif

	char process[256];
#ifdef SL_OS_WINDOWS
	SafeSprintf(process, sizeof(process)-1, "%s/shyloo.exe", sl::getAppPath());
	STARTUPINFO si = { sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = true;
	si.lpTitle = (char*)cmd;
	PROCESS_INFORMATION pi;
	BOOL ret = CreateProcess(process, (char*)cmd, nullptr, nullptr, false, CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi);
	SLASSERT(ret, "create process failed");
	::CloseHandle(pi.hThread);
	::CloseHandle(pi.hProcess);
	return 0;
#endif

#ifdef SL_OS_LINUX
	SafeSprintf(process, sizeof(process)-1, "%s/shyloo", sl::getAppPath());

	char args[MAX_CMD_LEN];
	SafeSprintf(args, sizeof(args), cmd);

	char *p[MAX_CMD_ARGS_COUNT];
	sl::SafeMemset(p, sizeof(p), 0, sizeof(p));
	p[0] = "shyloo";
	int32 idx = 1;
	char * checkPtr = args;
	char * innerPtr = nullptr;
	while((p[idx] = strtok_r(checkPtr, " ", &innerPtr)) != nullptr){
		++idx;
		checkPtr = nullptr;
	}

	pid_t pid;
	pid =fork();
	if (pid < 0){
		return pid;
	}
	else if(pid == 0){
		execv(process, p);
		return 0;
	}else
		return pid;
#endif
}

void Slave::onTime(sl::api::IKernel* pKernel, int64 timetick){
	float cpuPer = Machine::getInstance()->getCPUPer();
	printf("Slave Machine cpuper :%f\n", cpuPer);
}
