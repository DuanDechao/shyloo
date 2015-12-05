//�������Ӳ�������
/********************************************************************
	created:	2015/12/03
	created:	3:12:2015   20:06
	filename: 	e:\myproject\shyloo\netsvr\netsvr.cpp
	file path:	e:\myproject\shyloo\netsvr
	file base:	netsvr
	file ext:	cpp
	author:		ddc
	
	purpose:	�������Ӳ�������
*********************************************************************/

#include <stdio.h>
#include "../sllib/slsvr_base_frame.h"
#include "netctrl.h"
#include "../sllib/slserver_utils.h"
#include "netconfig.h"
using namespace sl;

#ifndef _DEBUG
#define THIS_BUILD "debug"
#else
#define THIS_BUILD "release"
#endif

void PrintVersion()
{

}

void PrintUsage()
{

}

int main(int argc, char** argv)
{
	PrintVersion();
	int iRet = 0;
	bool bDaemon = true;   ///Ĭ�Ϻ�ִ̨��
	bool iOpt = 0;

	while( (iOpt == getopt(argc, argv, "vghc:")) != -1)
	{
		switch(iOpt)
		{
		case 'v':
			return 0;
		case 'g':
			bDaemon = false;
			break;;
		case 'h':
			PrintUsage();
			return 0;
		case 'c':
			//ָ�������ļ���
			CONF->ConfigName = optarg;
			break;
		}
	}

	///��̨����
	if(bDaemon)
	{
		CServerUtils::InitDaemon();
	}

#if defined(_DEBUG)
	if(!bDaemon)
	{

	}
#endif

	CNetCtrl ctrl;
	iRet = ctrl.Init();

	iRet = ctrl.Run();

	iRet = ctrl.Exit();
	
	return 0;
}