#include "StartUpHandler.h"
#include "EventID.h"
#include "Monitor.h"
#include "ICluster.h"

StartUpHandler::StartUpHandler(){
	_serverReady = false;
	_serverReadyForLogin = false;
	sl::api::IKernel* pKernel = Monitor::getInstance()->getKernel();
	START_TIMER(this, 1000,	TIMER_BEAT_FOREVER, 1000);
}
void StartUpHandler::onTime(sl::api::IKernel* pKernel, int64 timetick){
	if(process()){
		pKernel->killTimer(this);
		ECHO_ERROR("server is ready for login");
	}
	else{
		ECHO_ERROR("server is not ready for login, retry....");
	}
}

bool StartUpHandler::process(){
	if(!SLMODULE(Cluster)->clusterIsReady()){
		ECHO_TRACE("cluster is not ready......");
		return false;
	}
	if(!_serverReady){
		_serverReady = Monitor::getInstance()->serverReady();
	}
	if(!_serverReadyForLogin){
		_serverReadyForLogin = Monitor::getInstance()->serverReadyForLogin();
	}

	return _serverReady && _serverReadyForLogin;
}
