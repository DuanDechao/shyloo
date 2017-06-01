#include "OCCommand.h"
#include "IDCCenter.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "slargs.h"
#include "IObjectLocator.h"

bool OCCommand::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	return true;
}

bool OCCommand::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() == NodeType::LOGIC){
		FIND_MODULE(_objectMgr, ObjectMgr);

		RGS_NODE_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_COMMAND, OCCommand::onCommand);
		RGS_NODE_HANDLER(_harbor, NodeProtocol::RELATION_MSG_FORWARD_COMMAND, OCCommand::onCommand);
	}

	if (_harbor->getNodeType() == NodeType::RELATION){
		FIND_MODULE(_objectLocator, ObjectLocator);

		RGS_NODE_HANDLER(_harbor, NodeProtocol::LOGIC_MSG_FORWARD_COMMAND, OCCommand::onLogicForwardCommand);
		
	}
	return true;
}

bool OCCommand::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void OCCommand::rgsCommand(int32 cmdId, const CMD_CB_TYPE& func, const char* debug){
	_cmdCBs.Register(cmdId, func, debug);
}

bool OCCommand::command(int32 cmdId, IObject* sender, int64 receiver, const OArgs& args){
	if (_harbor->getNodeType() != NodeType::LOGIC){
		SLASSERT(false, "cmd on invaild server node");
		return false;
	}

	return command(cmdId, sender->getID(), receiver, args);
}

bool OCCommand::command(int32 cmdId, int64 sender, int64 receiver, const OArgs& args){
	if (_harbor->getNodeType() != NodeType::LOGIC){
		SLASSERT(false, "cmd on invaild server node");
		return false;
	}

	IObject* receiverObj = _objectMgr->findObject(receiver);
	if (receiverObj){
		if (!receiverObj->isShadow()){
			_cmdCBs.Call(cmdId, _kernel, sender, receiverObj, args);
		}
		else{
			int32 logic = receiverObj->getPropInt32(attr_def::logic);
			SLASSERT(logic != _harbor->getNodeId(), "wtf");
			_harbor->prepareSend(NodeType::LOGIC, logic, NodeProtocol::LOGIC_MSG_COMMAND, sizeof(sender) + sizeof(cmdId) + sizeof(receiver) + args.getSize());
			_harbor->send(NodeType::LOGIC, logic, &cmdId, sizeof(cmdId));
			_harbor->send(NodeType::LOGIC, logic, &sender, sizeof(sender));
			_harbor->send(NodeType::LOGIC, logic, &receiver, sizeof(receiver));
			_harbor->send(NodeType::LOGIC, logic, args.getContext(), args.getSize());
		}
	}
	else{
		_harbor->prepareSend(NodeType::RELATION, 1, NodeProtocol::LOGIC_MSG_FORWARD_COMMAND, sizeof(sender)+sizeof(cmdId)+sizeof(receiver)+args.getSize());
		_harbor->send(NodeType::RELATION, 1, &cmdId, sizeof(cmdId));
		_harbor->send(NodeType::RELATION, 1, &sender, sizeof(sender));
		_harbor->send(NodeType::RELATION, 1, &receiver, sizeof(receiver));
		_harbor->send(NodeType::RELATION, 1, args.getContext(), args.getSize());
	}

	return true;
}

bool OCCommand::commandToPlayer(int32 cmdId, int32 logic, int64 receiver, const OArgs& args){
	if (_harbor->getNodeType() == NodeType::LOGIC){
		SLASSERT(false, "cmd on invailed server node type");
		return false;
	}

	int64 sender = 0;
	_harbor->prepareSend(NodeType::LOGIC, logic, NodeProtocol::RELATION_MSG_FORWARD_COMMAND, 2*sizeof(int64)+sizeof(int32) + args.getSize());
	_harbor->send(NodeType::LOGIC, logic, &cmdId, sizeof(cmdId));
	_harbor->send(NodeType::LOGIC, logic, &sender, sizeof(sender));
	_harbor->send(NodeType::LOGIC, logic, &receiver, sizeof(receiver));
	_harbor->send(NodeType::LOGIC, logic, args.getContext(), args.getSize());
	
	return true;
}

bool OCCommand::commandToPlayer(int32 cmdId, int64 receiver, const OArgs& args){
	if (_harbor->getNodeType() == NodeType::LOGIC){
		SLASSERT(false, "cmd on invailed server node type");
		return false;
	}
	
	if (_harbor->getNodeType() == NodeType::RELATION){
		int32 logic = _objectLocator->findObjectLogic(receiver);
		if (logic == game::NODE_INVALID_ID)
			return false;

		return commandToPlayer(cmdId, logic, receiver, args);
	}
	else{
		int64 sender = 0;
		_harbor->prepareSend(NodeType::RELATION, 1, NodeProtocol::LOGIC_MSG_FORWARD_COMMAND, sizeof(sender)+sizeof(cmdId)+sizeof(receiver)+args.getSize());
		_harbor->send(NodeType::RELATION, 1, &cmdId, sizeof(cmdId));
		_harbor->send(NodeType::RELATION, 1, &sender, sizeof(sender));
		_harbor->send(NodeType::RELATION, 1, &receiver, sizeof(receiver));
		_harbor->send(NodeType::RELATION, 1, args.getContext(), args.getSize());
	}

	return true;
}

void OCCommand::onCommand(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	const char* context = (const char*)args.getContext();
	const int32 size = args.getSize();
	SLASSERT(size >= sizeof(int32)+2 * sizeof(int64), "wtf");
	
	int32 cmdId = *(int32*)context;
	int64 sender = *(int64*)(context + sizeof(int32));
	int64 receiver = *(int64*)(context + sizeof(int32)+sizeof(int64));
	
	IObject* receiverObj = _objectMgr->findObject(receiver);
	if (!receiverObj || receiverObj->isShadow()){
		SLASSERT(false, "wtf");
		return;
	}

	int32 dataOffset = sizeof(int32)+2*sizeof(int64);
	_cmdCBs.Call(cmdId, _kernel, sender, receiverObj, OArgs(context + dataOffset, size - dataOffset));
}

void OCCommand::onLogicForwardCommand(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	const char* context = (const char*)args.getContext();
	const int32 size = args.getSize();
	SLASSERT(size >= sizeof(int32)+2 * sizeof(int64), "wtf");

	int32 cmdId = *(int32*)context;
	int64 sender = *(int64*)(context + sizeof(int32));
	int64 receiver = *(int64*)(context + sizeof(int32)+sizeof(int64));

	int32 logic = _objectLocator->findObjectLogic(receiver);
	if (logic == game::NODE_INVALID_ID)
		return;

	_harbor->prepareSend(NodeType::LOGIC, logic, NodeProtocol::RELATION_MSG_FORWARD_COMMAND, size);
	_harbor->send(NodeType::LOGIC, logic, context, size);
}

