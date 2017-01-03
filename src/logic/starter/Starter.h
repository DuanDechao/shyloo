#ifndef SL_LOGIC_STARTER_H
#define SL_LOGIC_STARTER_H
#include "slikernel.h"
#include "slimodule.h"
#include "IHarbor.h"
#include <unordered_map>
class Starter : public sl::api::IModule, public INodeListener
{
public:
	struct Execute{
		char name[64];
		int32 type;
		int32 min;
		int32 max;
	};

	struct Node{
		bool online;
		int32 max;
		int64 closeTick;
	};
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	static void startTimerInit(sl::api::IKernel * pKernel, int32 type);
	static void startTimerOnTime(sl::api::IKernel * pKernel, int32 type);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

private:
	void startNewNode(int32 nodeType, int32 nodeId, int32 slave);

private:
	static Starter*					s_self;
	static sl::api::IKernel*		s_kernel;
	static IHarbor*					s_harbor;

	static std::unordered_map<int32, Execute> s_executes;
	static std::unordered_map<int32, Node> s_nodes;

	static int64					s_checkInterval;
	static int64					s_deadTime;
};
#endif