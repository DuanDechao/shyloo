#ifndef _SL_INTERFACE_PACKETSENDER_H__
#define _SL_INTERFACE_PACKETSENDER_H__
#include "slimodule.h"
class IPacketSender : public sl::api::IModule{
public:
	virtual ~IPacketSender() {}
};

#endif