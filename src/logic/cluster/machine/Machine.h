#ifndef SL_MACHINE_H
#define SL_MACHINE_H
extern "C"
{
#include "../sigar/linux/sigar.h"
#include "../sigar/linux/sigar_format.h"
}
#include "slmulti_sys.h"
#include "slimodule.h"
#include "slsingleton.h"
class Machine : public sl::api::IModule, public sl::SLHolder<Machine> {
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
	
	float getCPUPer();
	float getCPUPerByPID(uint32 pid);
	void update();

private:
	bool autoCreate();
	bool clear();
	bool hasPID(int32 pid, sigar_proc_list_t* proclist);

private:
	Machine*				_self;
	sigar_t*				_sigar_cpu;
	sigar_cpu_t				_old_cpu;
	sigar_t*				_sigar_proc_list;
	sigar_proc_list_t		_proclist;

};
#endif
