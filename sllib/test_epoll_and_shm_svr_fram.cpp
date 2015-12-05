//#include "slepoll_and_shm_svr_fram.h"
//using namespace sl;
//class CAppCtrl: public CEpollAndShmSvr
//{
//public:
//	CAppCtrl():CEpollAndShmSvr("appsvr"){}
//	~CAppCtrl() {}
//public:
//	int LoadConfig()
//	{
//		CDataPathPara stPar(1, true, "../key/net2app.key", 1024, "../key/net2app.front.sock", "../key/net2app.back.sock", OnClientEvent);
//		InsertDataPath(stPar);
//		return 0;
//	}
//	//Ð´ÈëÊý¾Ý
//	static void OnClientEvent(unsigned int uiPathKey, int iEvent)
//	{
//		return;
//	}
//	int SendData(unsigned int uiDPKey, char* pszBuffer, int iBufLen);
//
//};
//int main(int argv, int **argc)
//{
//	CAppCtrl app;
//	app.LoadConfig();
//
//	return 0;
//}