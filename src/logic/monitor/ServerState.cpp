#include "ServerState.h"
#include "NodeProtocol.h"
#include "slargs.h"
#include "slxml_reader.h"

bool ServerState::initialize(sl::api::IKernel * pKernel){
	return true;
}

bool ServerState::launched(sl::api::IKernel * pKernel){
	return true;
}

bool ServerState::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}
