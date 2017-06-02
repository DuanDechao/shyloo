#ifndef SL_LOGIC_SLAVE_H
#define SL_LOGIC_SLAVE_H
#include "slikernel.h"
#include "slimodule.h"
#include <unordered_map>
class IHarbor;
class OArgs;
class Slave :public sl::api::IModule
{
public:
	struct CMD_INFO{
		char cmd[256];
	};

	struct EXECUTE_INFO{
		char name[64];
		char cmd[256];
	};

	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void openNewNode(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
	void startNewNode(sl::api::IKernel* pKernel, const char* name, const char* cmd, const int32 nodeType, const int32 nodeId);
	int32 startNode(sl::api::IKernel* pKernel, const char* cmd);

private:
	IHarbor* _harbor;

	int32	_startPort;
	int32	_endPort;
	int32	_startOutPort;
	int32   _endOutPort;
	int32   _balancePort;
	std::unordered_map<int64, CMD_INFO> _cmds;
	std::unordered_map<int32, EXECUTE_INFO> _executes;
};

#endif