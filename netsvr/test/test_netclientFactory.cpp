//#include "netdef.h"
//#include "netclient.h"
//#include "netclientfactory.h"
//#include "netconfig.h"
//using namespace sl;
//int main()
//{
//	CONF->LoadConfig();
//	CNetClientFactory m_stClientFactory;
//	int iClientSize = m_stClientFactory.CountSize(CONF->SocketMaxCount);
//	char *pBuffer = new char[iClientSize];
//	int SocketCount = CONF->SocketMaxCount;
//	memset(pBuffer, 0, iClientSize);
//	//初始化工厂
//	int iRet = m_stClientFactory.Init(pBuffer, iClientSize, CONF->SocketMaxCount);
//	if(iRet < 0)
//	{
//		return 0;
//	}
//	CNetClient* pstClient = m_stClientFactory.Alloc();
//	int i = m_stClientFactory.CalcObjectPos(pstClient);
//	printf("Handle: %d\n", i);
//	return 0;
//}