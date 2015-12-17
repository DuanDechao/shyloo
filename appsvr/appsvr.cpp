#include "../sllib/slsvr_base_frame.h"
#include "../sllib/slserver_utils.h"
#include "../sllib/slcode_convert.h"
#include <time.h>

#ifdef _DEBUG
#define THIS_BUILD "debug"
#else
#define THIS_BUILD "release"
#endif

using namespace sl;

void AtExit()
{
	SL_INFO("AT EXIT");
	if(PSVRCTRL)
	{
		CServerUtils::RemovePidFile(PSVRCTRL->GetSvrName());
	}
}

int main(int agrc, char** argv)
{
	tzset();
	bool bDaemon = true;  //Ĭ�J��̨����
	if(PSVRCTRL == NULL)
	{
		SL_ERROR("app lost CTrl obj!");
		return 0;
	}

#ifndef SL_OS_WINDOWS
	if (bDaemon)
	{
		CServerUtils::InitDaemon();
	}
#endif
	

	//���↢�Ӷ����M��
	if(!CServerUtils::CheckSingleRun(argv[0]))
	{
		return -1;
	}

	//�˳��r�Ԅ�����
	atexit(AtExit);

	PSVRCTRL->Init();
	PSVRCTRL->Run();
	PSVRCTRL->Exit();

	return 0;
}