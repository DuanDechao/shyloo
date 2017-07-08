#include "Slave.h"
#include "sltools.h"
#include <string>
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "slxml_reader.h"
#include "slargs.h"
#include "sltools.h"

#ifdef SL_OS_LINUX
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#endif

#define EXECUTE_CMD_PORT			"$port$"
#define EXECUTE_CMD_OUT_PORT		"$out_port$"
#define EXECUTE_CMD_BALANCE_PORT	"$balance_port$"
#define EXECUTE_CMD_ID				"$id$"
#define EXECUTE_CMD_PORT_SIZE		6
#define EXECUTE_CMD_OUT_PORT_SIZE	10
#define EXECUTE_CMD_BALANCE_PORT_SIZE	14
#define EXECUTE_CMD_ID_SIZE			4

#define NODE_CHECK_INTERVAL			500

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
				TRACE_LOG("process [%s] is lost, try restart", _node.cmd);
				_node.process = Slave::getInstance()->startNode(pKernel, _node.cmd);
			}
			else{
				ECHO_ERROR("wait pid %s failed[%d]", _node.cmd, SL_ERRNO);
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
	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "load core file %s failed", pKernel->getCoreFile());
		return false;
	}

	const sl::xml::ISLXmlNode& port = server_conf.root()["starter"][0]["port"][0];
	_startPort = port.getAttributeInt32("start");
	_endPort = port.getAttributeInt32("end");
	const sl::xml::ISLXmlNode& outPort = server_conf.root()["starter"][0]["out_port"][0];
	_startOutPort = outPort.getAttributeInt32("start");
	_endOutPort = outPort.getAttributeInt32("end");
	_balancePort = server_conf.root()["starter"][0]["define"][0].getAttributeInt32("balance_port");
	const sl::xml::ISLXmlNode& nodes = server_conf.root()["starter"][0]["node"];
	for (int32 i = 0; i < nodes.count(); i++){
		int32 type = nodes[i].getAttributeInt32("type");
		SafeSprintf(_executes[type].name, sizeof(_executes[type].name), "%s", nodes[i].getAttributeString("name"));
		SafeSprintf(_executes[type].cmd, sizeof(_executes[type].cmd), "%s", nodes[i].getAttributeString("cmd"));
	}

	return true;
}

bool Slave::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);

	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::MASTER_MSG_START_NODE, Slave::openNewNode);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::MASTER_MSG_STOP_NODES, Slave::stopNodes);

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

void Slave::openNewNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	int32 newNodeType = args.getInt32(0);
	int32 newNodeId = args.getInt32(1);
	TRACE_LOG("open new Node [%d:%d]", newNodeType, newNodeId);
	SLASSERT(_executes.find(newNodeType) != _executes.end(), "unknown nodetype %d", newNodeType);
	if (_executes.find(newNodeType) != _executes.end()){
		int64 node = (((int64)newNodeType) << 32) | newNodeId;
		auto iter = _cmds.find(node);
		if (iter != _cmds.end())
			startNode(pKernel, iter->second.cmd);
		else
			startNewNode(pKernel, _executes[newNodeType].name, _executes[newNodeType].cmd, newNodeType, newNodeId);
	}
}

void Slave::stopNodes(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	for (auto itor = _cmds.begin(); itor != _cmds.end(); ++itor){
		if (itor->second.timer){
			pKernel->killTimer(itor->second.timer);
			itor->second.timer = nullptr;
		}
	}
}

void Slave::startNewNode(sl::api::IKernel* pKernel, const char* name, const char* cmd, const int32 nodeType, const int32 nodeId){
	std::string tmp(cmd);
	std::string::size_type pos = tmp.find(EXECUTE_CMD_PORT);
	while (pos != std::string::npos){
		SLASSERT(_startPort <= _endPort, "wtf");
		if (_startPort > _endPort)
			return;
		
		char portStr[64];
		SafeSprintf(portStr, sizeof(portStr), "%d", _startPort++);
		tmp.replace(pos, EXECUTE_CMD_PORT_SIZE, portStr);
		pos = tmp.find(EXECUTE_CMD_PORT);
	}

	pos = tmp.find(EXECUTE_CMD_OUT_PORT);
	while (pos != std::string::npos){
		SLASSERT(_startOutPort <= _endOutPort, "wtf");
		if (_startOutPort > _endOutPort)
			return;
		
		char outPortStr[64];
		SafeSprintf(outPortStr, sizeof(outPortStr), "%d", _startOutPort++);
		tmp.replace(pos, EXECUTE_CMD_OUT_PORT_SIZE, outPortStr);
		pos = tmp.find(EXECUTE_CMD_OUT_PORT);
	}

	pos = tmp.find(EXECUTE_CMD_BALANCE_PORT);
	if (pos != std::string::npos){
		char balancePort[64];
		SafeSprintf(balancePort, sizeof(balancePort), "%d", _balancePort);
		tmp.replace(pos, EXECUTE_CMD_BALANCE_PORT_SIZE, balancePort);
	}

	pos = tmp.find(EXECUTE_CMD_ID);
	if (pos != std::string::npos){
		char idStr[64];
		SafeSprintf(idStr, sizeof(idStr), "%d", nodeId);
		tmp.replace(pos, EXECUTE_CMD_ID_SIZE, idStr);
	}

	int64 node = (((int64)nodeType) << 32) | nodeId;
	SafeSprintf(_cmds[node].cmd, sizeof(_cmds[node].cmd), "%s", tmp.c_str());

	_cmds[node].process = startNode(pKernel, _cmds[node].cmd);

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
