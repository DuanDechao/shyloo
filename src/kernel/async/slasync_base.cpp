#include "slasync_base.h"
#include "slkernel.h"
namespace sl
{
namespace core
{
void AsyncBase::onExecute(){
	if (_valid){
		_successd = _handler->onExecute(core::Kernel::getInstance());
		_executed = true;
	}
}

void AsyncBase::onComplete(){
	if (_successd)
		_handler->onSuccess(Kernel::getInstance());
	else
		_handler->onFailed(Kernel::getInstance(), _executed);

	_handler->onRelease(Kernel::getInstance());
}

void AsyncBase::release(){
	DEL this;
}

}
}
