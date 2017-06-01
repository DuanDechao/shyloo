#ifndef _SL_INTERFACE_OCCOMMAND_H__
#define _SL_INTERFACE_OCCOMMAND_H__
#include "slimodule.h"
#include "slbinary_stream.h"

class IObject;
typedef std::function<void(sl::api::IKernel*, int64, IObject*, const OArgs&)> CMD_CB_TYPE;

class IOCCommand : public sl::api::IModule{
public:
	virtual ~IOCCommand() {}
	
	virtual void rgsCommand(int32 cmdId, const CMD_CB_TYPE& func, const char* debug) = 0;

	virtual bool command(int32 cmdId, int64 sender, int64 receiver, const OArgs& args) = 0;
	virtual bool command(int32 cmdId, IObject* sender, int64 receiver, const OArgs& args) = 0;

	//不要在logic上使用
	virtual bool commandToPlayer(int32 cmdId, int64 receiver, const OArgs& args) = 0;
	virtual bool commandToPlayer(int32 cmdId, int32 logic, int64 receiver, const OArgs& args) = 0;
};

#define RSG_COMMAND_HANDLER(command, cmdId, cb) command->rgsCommand(cmd, cb, #cb) 

#endif