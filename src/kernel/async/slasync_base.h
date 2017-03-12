#ifndef __SL_ASYNC_BASE_H__
#define __SL_ASYNC_BASE_H__
#include "slikernel.h"
namespace sl
{
namespace core
{

class AsyncBase : public api::IAsyncBase{
public:
	AsyncBase(api::IAsyncHandler* handler, const char* debug) 
		: _handler(handler)
	{
		SafeSprintf(_debug, sizeof(_debug), "%s", debug);
		_handler->setBase(this);

		_valid = true;
		_executed = false;
		_successd = false;
	}

	virtual ~AsyncBase() {}

	inline void setInVaild() { _valid = false; }

	void onExecute();
	void onComplete();
	void release();
	 
private:
	char _debug[256];
	api::IAsyncHandler*  _handler;

	bool _valid;
	bool _executed;
	bool _successd;
};
}
}

#endif