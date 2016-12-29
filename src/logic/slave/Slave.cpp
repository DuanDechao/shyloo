#include "Slave.h"
#include "sltools.h"
#include <string>
#define EXECUTE_CMD_PORT			"$port$"
#define EXECUTE_CMD_OUT_PORT		"$out_port$"
#define EXECUTE_CMD_ID				"$id$"
#define EXECUTE_CMD_PORT_SIZE		6
#define EXECUTE_CMD_OUT_PORT_SIZE	10
#define EXECUTE_CMD_ID_SIZE			4

void Slave::openNewNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const char* pContext, const int32 size){
	SLASSERT(size >= (sizeof(int32)+sizeof(int32)), "wtf");
	int32 newNodeType = *(int32*)pContext;
	int32 newNodeId = *(int32*)(pContext + sizeof(int32));
	
	SLASSERT(s_executes.find(newNodeType) != s_executes.end(), "unknown nodetype %d", newNodeType);
	if (s_executes.find(newNodeType) != s_executes.end()){
		int64 node = (((int64)newNodeType) << 32) | nodeId;
		auto iter = s_cmds.find(node);
		if (iter != s_cmds.end())
			startNode(pKernel, iter->second.cmd);
		else
			startNewNode(pKernel, s_executes[newNodeType].name, s_executes[newNodeType].cmd, newNodeType, newNodeId);
	}
}

void Slave::startNewNode(sl::api::IKernel* pKernel, const char* name, const char* cmd, const int32 nodeType, const int32 nodeId){
	std::string tmp(cmd);
	std::string::size_type pos = tmp.find(EXECUTE_CMD_PORT);
	while (pos != std::string::npos){
		SLASSERT(s_startPort <= s_endPort, "wtf");
		if (s_startPort > s_endPort)
			return;
		
		char portStr[64];
		SafeSprintf(portStr, sizeof(portStr), "%d", s_startPort++);
		tmp.replace(pos, EXECUTE_CMD_PORT_SIZE, portStr);
		pos = tmp.find(EXECUTE_CMD_PORT);
	}

	pos = tmp.find(EXECUTE_CMD_OUT_PORT);
	while (pos != std::string::npos){
		SLASSERT(s_startOutPort <= s_endOutPort, "wtf");
		if (s_startOutPort > s_endOutPort)
			return;
		
		char outPortStr[64];
		SafeSprintf(outPortStr, sizeof(outPortStr), "%d", s_startOutPort++);
		tmp.replace(pos, EXECUTE_CMD_OUT_PORT_SIZE, outPortStr);
		pos = tmp.find(EXECUTE_CMD_OUT_PORT);
	}

	pos = tmp.find(EXECUTE_CMD_ID);
	if (pos != std::string::npos){
		char idStr[64];
		SafeSprintf(idStr, sizeof(idStr), "%d", nodeId);
		tmp.replace(pos, EXECUTE_CMD_ID_SIZE, idStr);
	}

	int64 node = (((int64)nodeType) << 32) | nodeId;
	SafeSprintf(s_cmds[node].cmd, sizeof(s_cmds[node].cmd), tmp.c_str());
	
	startNode(pKernel, s_cmds[node].cmd);
}
int32 Slave::startNode(sl::api::IKernel* pKernel, const char* cmd){
	char process[256];
#ifdef SL_OS_WINDOWS
	SafeSprintf(process, sizeof(process)-1, "%s/shyloo.exe", sl::getAppPath());
	STARTUPINFO si = { sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.lpTitle = (char*)cmd;
	PROCESS_INFORMATION pi;
	BOOL ret = CreateProcess(process, (char*)cmd, nullptr, nullptr, false, CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi);
	SLASSERT(ret, "create process failed");
	::CloseHandle(pi.hThread);
	::CloseHandle(pi.hProcess);
	return 0;
#endif

#ifdef SL_OS_LINUX

#endif
}