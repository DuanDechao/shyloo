#ifndef _SL_INTERFACE_LOGIC_H__
#define _SL_INTERFACE_LOGIC_H__
#include "slimodule.h"
#include "slbinary_stream.h"
class IObject;
class IProtocolHandler{
public:
	virtual ~IProtocolHandler() {}
	virtual bool dealProtocol(sl::api::IKernel* pKernel, IObject* object, const void* context, const int32 size) = 0;
};

typedef std::function<bool(sl::api::IKernel* pKernel, IObject* object, const sl::OBStream& args)> HandleFunctionType;
class BProtocolHandler : public IProtocolHandler{
public:
	BProtocolHandler(const HandleFunctionType func) : _func(func) {}

	virtual bool dealProtocol(sl::api::IKernel* pKernel, IObject* object, const void* context, const int32 size){
		return _func(pKernel, object, sl::OBStream((const char*)context, size));
	}

private:
	HandleFunctionType _func;
};

class ILogic : public sl::api::IModule{
public:
	virtual ~ILogic() {}

	void rgsProtocolHandler(int32 messageId, const HandleFunctionType& f, const char* debug){
		rgsProtocolHandler(messageId, NEW BProtocolHandler(f), debug);
	}

protected:
	virtual void rgsProtocolHandler(int32 messageId, IProtocolHandler* handler, const char* debug) = 0;
};

#define RGS_PROTO_HANDLER(logic, messageId, cb) logic->rgsProtocolHandler(messageId, std::bind(&cb, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3), #cb)

#endif