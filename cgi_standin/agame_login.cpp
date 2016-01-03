#include "../sllib/slcode_convert.h"
#include "../sllib/slmsg_base.h"
#include "slclient_inf.h"
#include "define.h"
#include "../sllib/sltime.h"
using namespace sl;

int main()
{
	int iRet;
	iRet = SL_CLIENT->Init();
	if(iRet)
	{
		printf("init error, iRet=%d\n", iRet);
	}
	CReqHeartbeat stReq;
	CClientHeartbeatCmd heartbeatCmd;
	heartbeatCmd.Init(0, stReq);
	heartbeatCmd.Do(NULL);
	while (!SL_CLIENT->Recv());
	CTime stNow((time_t)heartbeatCmd.m_Ans.m_iNow);
	char szTimeBuf[32] = {0};
	stNow.Format(SL_STRSIZE(szTimeBuf), "%Y-%m-%d %H:%M:%S");
	printf("time:%s\n", szTimeBuf);
	return 0;
}