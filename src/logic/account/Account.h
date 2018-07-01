#ifndef __SL_FRAMEWORK_ACCOUNT_H__
#define __SL_FRAMEWORK_ACCOUNT_H__
#include "slikernel.h"
#include "slimodule.h"
#include "IAccount.h"
#include "slsingleton.h"
#include <unordered_map>
#include <unordered_set>

class IHarbor;
class Account :public IAccount, public sl::SLHolder<Account>{
	enum {
		ACCOUNT_STATE_OFFLINE = 0,
		ACCOUNT_STATE_ONLINE,
		ACCOUNT_STATE_SWITCH,
	};

	struct AccountInfo{
		int64 accountId;
		int8 state;
		int32 gateId;
		int64 agentId;
		int32 switchGateId;
		int64 switchAgentId;
	};
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	void onGateBindAccount(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onGateUnBindAccount(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

private:
	void bindAccountSuccess(sl::api::IKernel* pKernel, const int32 nodeType, const AccountInfo& account);

private:
	Account* _self;
	sl::api::IKernel* _kernel;
	IHarbor* _harbor;

	std::unordered_map<int64, AccountInfo> _accounts;
	std::unordered_map<int32, std::unordered_set<int64>> _gateAccounts;
	std::unordered_map<int32, std::unordered_set<int64>> _switchGateAccounts;

};
#endif
