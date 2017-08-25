#ifndef SL_LOGIC_STARTER_H
#define SL_LOGIC_STARTER_H
#include "slikernel.h"
#include "IStarter.h"
#include <unordered_map>
class IStarter;
class StartNodeStrategy : public sl::api::IModule, public IStartStrategy{
	enum{
		OVERLOAD = 0,
		BANDWIDTH,
		
		DATA_COUNT,
	};

	struct Score{
		int32 forceNode;
		int32 value[DATA_COUNT];
	};

public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void addSlave(const int32 nodeId);
	virtual int32 chooseNode(const int32 nodeType, const int32 nodeId);

private:
	int32 choose(sl::api::IKernel* pKernel, int32 nodeType, int32 first, int32 second);

private:
	StartNodeStrategy*		_self;
	sl::api::IKernel*		_kernel;
	IStarter*				_starter;

	std::unordered_map<int32, Score> _scores;
	std::unordered_map<int32, Score> _slaves;
};
#endif