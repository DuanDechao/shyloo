//ÐÄÌø°ü
#include "../sllib/slapp_so_cmd.h"
#include "account_msg.h"
using namespace sl;
class CCmdHeartbeat: public CAppSoClientCmd<CReqHeartbeat, CAnsHeartbeat, CReqHeartbeat::MSG_ID>
{
public:
	int DoWork()
	{
		m_stAns.m_iNow = static_cast<int>(time(NULL));

		return 0;
	}
};

SL_REGISTER_ASYNCCMD(CCmdHeartbeat::CMD_ID, CCmdHeartbeat);