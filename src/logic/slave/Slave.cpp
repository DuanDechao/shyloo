#include "Slave.h"
#include "sltools.h"
#include <string>
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "slxml_reader.h"
#include "slargs.h"

#define EXECUTE_CMD_PORT			"$port$"
#define EXECUTE_CMD_OUT_PORT		"$out_port$"
#define EXECUTE_CMD_ID				"$id$"
#define EXECUTE_CMD_PORT_SIZE		6
#define EXECUTE_CMD_OUT_PORT_SIZE	10
#define EXECUTE_CMD_ID_SIZE			4

IHarbor* Slave::s_harbor = nullptr;
int32 Slave::s_startPort = 0;
int32 Slave::s_endPort = 0;
int32 Slave::s_startOutPort = 0;
int32 Slave::s_endOutPort = 0;
std::unordered_map<int64, Slave::CMD_INFO> Slave::s_cmds;
std::unordered_map<int32, Slave::EXECUTE_INFO> Slave::s_executes;

bool Slave::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool Slave::launched(sl::api::IKernel * pKernel){
	s_harbor = (IHarbor*)pKernel->findModule("Harbor");
	SLASSERT(s_harbor, "not find module harbor");
	s_harbor->rgsNodeMessageHandler(NodeProtocol::MASTER_MSG_START_NODE, Slave::openNewNode);

	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "load core file %s failed", pKernel->getCoreFile());
		return false;
	}

	const sl::xml::ISLXmlNode& port = server_conf.root()["starter"][0]["port"][0];
	s_startPort = port.getAttributeInt32("start");
	s_endPort = port.getAttributeInt32("end");
	const sl::xml::ISLXmlNode& outPort = server_conf.root()["starter"][0]["out_port"][0];
	s_startOutPort = outPort.getAttributeInt32("start");
	s_endOutPort = outPort.getAttributeInt32("end");
	const sl::xml::ISLXmlNode& nodes = server_conf.root()["starter"][0]["node"];
	for (int32 i = 0; i < nodes.count(); i++){
		int32 type = nodes[i].getAttributeInt32("type");
		SafeSprintf(s_executes[type].name, sizeof(s_executes[type].name), "%s", nodes[i].getAttributeString("name"));
		SafeSprintf(s_executes[type].cmd, sizeof(s_executes[type].cmd), "%s", nodes[i].getAttributeString("cmd"));
	}
	return true;
}
bool Slave::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Slave::openNewNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	int32 newNodeType = args.getInt32(0);
	int32 newNodeId = args.getInt32(1);
	
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
	SafeSprintf(s_cmds[node].cmd, sizeof(s_cmds[node].cmd), "%s", tmp.c_str());
	
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