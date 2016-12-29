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
	};
	struct Node{
		bool online;
	};
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void startTimerInit(sl::api::IKernel * pKernel, int32 type);
	void startTimerOnTime(sl::api::IKernel * pKernel, int32 type);

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