#ifndef __CLIENT_DEFINE_H__
#define __CLIENT_DEFINE_H__
#include "slclient_so_cmd.h"
#include "../appsvr/account_msg.h"
namespace sl
{
	typedef  CClientSoCmd<CReqHeartbeat, 
						  CAnsHeartbeat, 
						  CReqHeartbeat::MSG_ID> CClientHeartbeatCmd;
}
#endif
