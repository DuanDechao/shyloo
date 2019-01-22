#include "slmulti_sys.h"
#include "slkernel.h"
using namespace sl::core;
int main(int argc, char ** argv){
	Kernel * pKernel = (Kernel *) Kernel::getInstance();
	if (pKernel == nullptr)
		pKernel = NEW Kernel();
	bool res = pKernel->initialize(argc, argv);
	if(res){
		pKernel->loop();
	}
	KERNEL_INFO("%s %s is shuting down", pKernel->getCmdArg("name"), pKernel->getCmdArg("node_id"));
	pKernel->destory();

	return 0;
}
