#include "Account.h"
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "AgentProtocol.h"
#include "NodeDefine.h"

bool Account::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Account::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	RGS_NODE_HANDLER(_harbor, NodeProtocol::GATE_MSG_BIND_ACCOUNT_REQ, Account::onGateBindAccount);
	return true;
}
bool Account::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Account::onGateBindAccount(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 agentId = args.getInt64(0);
	int64 accountId = args.getInt64(1);

	auto itor = _accounts.find(accountId);
	if (itor == _accounts.end()){
		_accounts[accountId] = { accountId, ACCOUNT_STATE_OFFLINE, 0, 0, 0, 0 };
	}

	AccountInfo& account = _accounts[accountId];
	switch (account.state){
	case ACCOUNT_STATE_OFFLINE : {
			 account.gateId = nodeId;
			 account.agentId = agentId;
			 account.state = ACCOUNT_STATE_ONLINE;

			 _gateAccounts[nodeId].insert(accountId);

			 bindAccountSuccess(pKernel, account);
		}
		break;
	case ACCOUNT_STATE_ONLINE:{
			account.state = ACCOUNT_STATE_SWITCH;
			account.switchGateId = nodeId;
			account.switchAgentId = agentId;
			_switchGateAccounts[nodeId].insert(accountId);

			IArgs<1, 128> args;
			args << account.agentId;
			args.fix();
			_harbor->send(NodeType::GATE, account.gateId, NodeProtocol::ACCOUNT_MSG_KICK_FROM_ACCOUNT, args.out());
		}
		break;
	case ACCOUNT_STATE_SWITCH:{
			IArgs<3, 128> args;
			args << account.switchAgentId << accountId << (int32)ProtocolError::ERROR_ACCOUNT_AUTHEN_FAILED;
			args.fix();
			_harbor->send(NodeType::GATE, account.switchGateId, NodeProtocol::ACCOUNT_MSG_BIND_ACCOUNT_ACK, args.out());

			_switchGateAccounts[account.switchGateId].erase(accountId);

			account.switchAgentId = agentId;
			account.switchGateId = nodeId;
			_switchGateAccounts[nodeId].insert(accountId);
		}
		break;
	}
}

void Account::bindAccountSuccess(sl::api::IKernel* pKernel, const AccountInfo& account){
	IArgs<4, 1024> args;
	args << account.agentId << account.accountId << (int32)ProtocolError::ERROR_NO_ERROR;
	args.fix();
	_harbor->send(NodeType::GATE, account.gateId, NodeProtocol::ACCOUNT_MSG_BIND_ACCOUNT_ACK, args.out());
}