#include "Account.h"
#include "IHarbor.h"
#include "NodeProtocol.h"
#include "NodeDefine.h"
#include "Protocol.pb.h"

bool Account::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool Account::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::GATE_MSG_BIND_ACCOUNT_REQ, Account::onGateBindAccount);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::GATE_MSG_UNBIND_ACCOUNT_REQ, Account::onGateUnBindAccount);
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

			 bindAccountSuccess(pKernel, nodeType, account);
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
			_harbor->send(nodeType, account.gateId, NodeProtocol::ACCOUNT_MSG_KICK_FROM_ACCOUNT, args.out());
		}
		break;
	case ACCOUNT_STATE_SWITCH:{
			IArgs<3, 128> args;
			args << account.switchAgentId << accountId << (int32)protocol::ErrorCode::ERROR_ACCOUNT_AUTHEN_FAILED;
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

void Account::onGateUnBindAccount(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int64 agentId = args.getInt64(0);
	int64 accountId = args.getInt64(1);

	auto itor = _accounts.find(accountId);
	if (itor == _accounts.end()){
		_accounts[accountId] = { accountId, ACCOUNT_STATE_OFFLINE, 0, 0, 0, 0 };
	}

	AccountInfo& account = _accounts[accountId];
	switch (account.state){
	case ACCOUNT_STATE_ONLINE:{
			if (account.gateId == nodeId && account.agentId == agentId){
				account.state = ACCOUNT_STATE_OFFLINE;
				account.agentId = 0;
				account.gateId = 0;
				_gateAccounts[nodeId].erase(accountId);
			}
		}
		break;
	case ACCOUNT_STATE_SWITCH:{
			if (account.gateId == nodeId && account.gateId == agentId){
				_gateAccounts[nodeId].erase(accountId);
				_switchGateAccounts[account.switchGateId].erase(accountId);
				account.agentId = account.switchAgentId;
				account.gateId = account.switchGateId;
				account.switchAgentId = 0;
				account.switchGateId = 0;
				account.state = ACCOUNT_STATE_ONLINE;
				_gateAccounts[account.gateId].insert(accountId);

				bindAccountSuccess(pKernel, nodeType, account);
			}
			else if(account.switchGateId == nodeId && account.switchAgentId == agentId){
				_switchGateAccounts[account.switchGateId].erase(accountId);
				account.switchGateId = 0;
				account.switchAgentId = 0;
				account.state = ACCOUNT_STATE_ONLINE;
			}
		}
		break;
	}
}

void Account::bindAccountSuccess(sl::api::IKernel* pKernel, const int32 nodeType, const AccountInfo& account){
	IArgs<4, 1024> args;
	args << account.agentId << account.accountId << (int32)protocol::ErrorCode::ERROR_NO_ERROR;
	args.fix();
	_harbor->send(nodeType, account.gateId, NodeProtocol::ACCOUNT_MSG_BIND_ACCOUNT_ACK, args.out());
}
