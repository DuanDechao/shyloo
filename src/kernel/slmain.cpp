#include "slmulti_sys.h"
#include "slkernel.h"
using namespace sl::core;
int main(int argc, char ** argv){
	Kernel * pKernel = (Kernel *) Kernel::getInstance();
	if (pKernel == nullptr)
		pKernel = NEW Kernel();
	SLASSERT(pKernel, "get kernel point error");
	bool res = pKernel->initialize(argc, argv);
	SLASSERT(res, "launch kernel error");

	if(res){
		pKernel->loop();
		pKernel->destory();
	}

	return 0;
}