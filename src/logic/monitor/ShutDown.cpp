#include "ShutDown.h"
#include "IHarbor.h"
#include "IMonitor.h"
#include "NodeDefine.h"
#include "MonitorProtocol.h"
#include "slxml_reader.h"
#include "sltools.h"
#include "slargs.h"
#include "NodeProtocol.h"
#include "EventID.h"
#include "IEventEngine.h"

bool ShutDown::initialize(sl::api::IKernel * pKernel){
	_currStep = -1;
	_shutdowning = false;
	
	if (!loadShutdownConfig(pKernel)){
		SLASSERT(false, "wtf");
		return false;
	}

	return true;
}

bool ShutDown::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_eventEngine, EventEngine);
	if (_harbor->getNodeType() == NodeType::MASTER){
		FIND_MODULE(_monitor, Monitor);

		_harbor->addNodeListener(this);

		RGS_MONITOR_ARGS_HANDLER(_monitor, MonitorProtocol::CLIENT_SHUTDOWN_SERVER_REQ, ShutDown::shutdownServerReq);
		RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::CLUSTER_MSG_SHUTDOWN_ACK, ShutDown::onClusterShutdownAck);
	}
	else{
		RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::MASTER_MSG_ASK_SHUTDOWN, ShutDown::onMasterAskShutdown);
		RGS_EVENT_HANDLER(_eventEngine, logic_event::EVENT_SHUTDOWN_COMPLETE, ShutDown::onClusterShutdownComplete);
	}

	return true;
}

bool ShutDown::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

bool ShutDown::loadShutdownConfig(sl::api::IKernel* pKernel){
	sl::XmlReader server_conf;
	if (!server_conf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "load core file %s failed", pKernel->getCoreFile());
		return false;
	}

	const sl::xml::ISLXmlNode& steps = server_conf.root()["shutdown"][0]["step"];
	for (int32 i = 0; i < steps.count(); i++){
		ShutDownStep info;
		info.optType = steps[i].getAttributeBoolean("close") ? OptType::CLOSE : OptType::NOTIFY;
		const sl::xml::ISLXmlNode& nodes = steps[i]["node"];
		for (int32 j = 0; j < nodes.count(); j++){
			int32 nodeType = nodes[j].getAttributeInt32("type");
			SelectType sType = strcmp(nodes[j].getAttributeString("select"), "random") == 0 ? SelectType::RANDOM : SelectType::ALL;
			info.nodes.push_back({ nodeType, sType });
		}
		_allSteps.push_back(info);
	}

	return true;
}

void ShutDown::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	_allNodes[nodeType].insert(nodeId);
}

void ShutDown::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	_allNodes[nodeType].erase(nodeId);

	if (_shutdowning){
		SLASSERT(_currStep < (int32)_allSteps.size() && _currStep >= 0, "wtf");
		if (_allSteps[_currStep].optType == OptType::CLOSE){
			_currOptNodes[nodeType].erase(nodeId);
			ECHO_TRACE("node[%s %d] has shutdown", _harbor->getNodeName(nodeType), nodeId);
			checkNextStep(pKernel);
		}
	}
}

void ShutDown::shutdownServerReq(sl::api::IKernel* pKernel, const int64 id, const sl::OBStream& args){
	terminate(pKernel);
}

void ShutDown::terminate(sl::api::IKernel* pKernel){
	SLASSERT(!_shutdowning, "wtf");
	_shutdowning = true;

	logic_event::PreShutDown evt;
	_eventEngine->execEvent(logic_event::EVENT_PRE_SHUTDOWN, &evt, sizeof(evt));

	startNewStep(pKernel);
}

void ShutDown::startNewStep(sl::api::IKernel* pKernel){
	if (_currStep + 1 < (int32)_allSteps.size()){
		_currStep++;

		for (auto node : _allSteps[_currStep].nodes){
			if (node.selectType == SelectType::RANDOM){
				int32 idx = sl::getRandom(0, (int32)_allNodes[node.nodeType].size() - 1);
				for (auto nodeId : _allNodes[node.nodeType]){
					if (idx == 0){
						_currOptNodes[node.nodeType].insert(nodeId);
						break;
					}
					--idx;
				}
			}
			else{
				_currOptNodes[node.nodeType] = _allNodes[node.nodeType];
			}
		}
		sendNewStep(pKernel);
		checkNextStep(pKernel);
	}
	else{
		pKernel->shutdown();
	}
}

void ShutDown::checkNextStep(sl::api::IKernel* pKernel){
	bool completed = true;
	for (auto nodeItor = _currOptNodes.begin(); nodeItor != _currOptNodes.end(); ++nodeItor){
		if (!nodeItor->second.empty()){
			completed = false;
			break;
		}
	}

	if (completed)
		startNewStep(pKernel);
}

void ShutDown::sendNewStep(sl::api::IKernel* pKernel){
	IArgs<2, 128> args;
	args << (int8)_currStep;
	args << (int8)_allSteps[_currStep].optType;
	args.fix();

	auto nodeItor = _currOptNodes.begin();
	for (; nodeItor != _currOptNodes.end(); ++nodeItor){
		for (auto nodeId : nodeItor->second){
			ECHO_ERROR("asking [%s %d] shutdowning", _harbor->getNodeName(nodeItor->first), nodeId);
			_harbor->send(nodeItor->first, nodeId, NodeProtocol::MASTER_MSG_ASK_SHUTDOWN, args.out());
		} 
	}
}

void ShutDown::onMasterAskShutdown(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int8 _currShutdownStep = args.getInt8(0);
	int8 _optType = args.getInt8(1);

	logic_event::ShutDown info{ _currShutdownStep };
	if (_optType == OptType::NOTIFY){
		_eventEngine->execEvent(logic_event::EVENT_SHUTDOWN_NOTIFY, &info, sizeof(info));
	}
	else if (_optType == OptType::CLOSE){
		_eventEngine->execEvent(logic_event::EVENT_SHUTDOWN_CLOSE, &info, sizeof(info));
		pKernel->shutdown();
		ECHO_ERROR("shutdowning......");
	}
	else{
		SLASSERT(false, "unknown opt Type");
	}
}

void ShutDown::onClusterShutdownAck(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int8 stepIdx = args.getInt8(0);
	if (_shutdowning){
		SLASSERT(stepIdx == _currStep, "wtf");
		if (stepIdx == _currStep){
			if (_allSteps[_currStep].optType == OptType::NOTIFY){
				_currOptNodes[nodeType].erase(nodeId);
				checkNextStep(pKernel);
			}
		}
	}
}

void ShutDown::onClusterShutdownComplete(sl::api::IKernel* pKernel, const void* context, const int32 size){
	SLASSERT(size == sizeof(logic_event::ShutDown), "wtf");
	logic_event::ShutDown* info = (logic_event::ShutDown*)context;
	
	IArgs<1, 32> args;
	args << info->step;
	args.fix();
	_harbor->send(NodeType::MASTER, 1, NodeProtocol::CLUSTER_MSG_SHUTDOWN_ACK, args.out());
}