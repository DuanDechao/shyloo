#include "Machine.h"

bool Machine::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_sigar_cpu = NULL;
	_sigar_proc_list = NULL;
	return true;
}

bool Machine::launched(sl::api::IKernel * pKernel){
	return true;
}

bool Machine::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Machine::update(){
	autoCreate();
}

float Machine::getCPUPer(){
	sigar_cpu_t current;
	if(_sigar_cpu == NULL)
		sigar_open(&_sigar_cpu);

	sigar_cpu_perc_t perc;
	sigar_cpu_get(_sigar_cpu, &current);
	sigar_cpu_perc_calculate(&_old_cpu, &current, &perc);
	_old_cpu = current;

	float ret = float(perc.combined) * 100.f;
	return ret;
}

float Machine::getCPUPerByPID(uint32 pid){
	if(pid== 0){
		pid = (uint32)getpid();
	}

	float percent = 0.f;
	bool tryed = false;

_TRYGET:
	if(!hasPID(pid, &_proclist)){
		if(!tryed){
			clear();
			tryed = true;

			if(autoCreate())
				goto _TRYGET;
		}
		return 0.f;
	}

	sigar_proc_cpu_t cpu;
	int32 status = sigar_proc_cpu_get(_sigar_proc_list, pid, &cpu);
	if(status == SIGAR_OK){
		percent = float(cpu.percent) * 100.f;
		//percent /= 
	}
	else{
		printf("Machine::getCPUPerByPID: error %d (%s) getCPUPerByPID(%d)", status, sigar_strerror(_sigar_proc_list, status), pid);
		return 0.f;
	}
	return percent;
}

bool Machine::autoCreate(){
	if(_sigar_proc_list = NULL){
		sigar_open(&_sigar_proc_list);
		int32 status = sigar_proc_list_get(_sigar_proc_list, &_proclist);
		if(status != SIGAR_OK){
			printf("Machine::autoCreate error %d(%s) sigar_proc_list_get", status, sigar_strerror(_sigar_proc_list, status));
			sigar_close(_sigar_proc_list);
			_sigar_proc_list = NULL;
			return false;
		}
	}
	return true;
}

bool Machine::clear(){
	if(_sigar_proc_list){
		sigar_proc_list_destroy(_sigar_proc_list, &_proclist);
		sigar_close(_sigar_proc_list);
		_sigar_proc_list = NULL;
	}
	if(_sigar_cpu){
		sigar_close(_sigar_cpu);
		_sigar_cpu = NULL;
	}

	return true;
}

bool Machine::hasPID(int32 pid, sigar_proc_list_t * proclist){
	for(int32 i = 0; i < proclist->number; i++){
		sigar_pid_t cpid = proclist->data[i];
		if(cpid == pid){
			sigar_proc_state_t procstate;
			if(sigar_proc_state_get(_sigar_proc_list, pid, &procstate) != SIGAR_OK){
				return false;
			}
			return true;
		}
	}
	return false;
}
