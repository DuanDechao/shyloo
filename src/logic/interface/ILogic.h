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

class BProtocolHandler{
public:
	typedef std::function<bool(sl::api::IKernel* pKernel, IObject* object, const sl::OBStream& args)> HandleFunctionType;
	BProtocolHandler
};

class ILogic : public sl::api::IModule{
public:
	virtual ~ILogic() {}
};

#endif