///net������
/********************************************************************
	created:	2015/12/02
	created:	2:12:2015   21:01
	filename: 	e:\myproject\shyloo\netsvr\netconfig.h
	file path:	e:\myproject\shyloo\netsvr
	file base:	netconfig
	file ext:	h
	author:		ddc
	
	purpose:	net������
*********************************************************************/

#ifndef _NET_CONFIG_H_
#define _NET_CONFIG_H_

#include "netdef.h"
namespace sl
{
	class CNetConfig
	{
	public:

		//�����ڴ�
		CSizeString<SL_PATH_MAX>		MgrShmKey;
		
		//ShmQueue������
		CSizeString<SL_PATH_MAX>		FrontEndShmKey;		///< ǰ�˹����ڴ��Key
		int								FrontEndShmSize;	///< ǰ�˹����ڴ�Ĵ�С
		CSizeString<SL_PATH_MAX>		FrontEndSocket;		///< ǰ��Net��UnixSocketFile
		CSizeString<SL_PATH_MAX>		BackEndSocket;		///< ��˵�UnixSocketFile

		//������
		int								RecvBufferSize;		///< SOCKET��������С
		int								SendBufferSize;		///< SOCKET��������С
		int								BufferCount;		///< SOCKET������������Recv��Send�Ļ���������BufferCount��

		//������
		int								SocketMaxCount;		///< ���������

		//�����˿���Ϣ
		CArray<CNetListenInfo, NET_MAX_LISTEN> ListenArray;

		///������������


		CSizeString<SL_PATH_MAX>		ConfigName;			///< �����ļ���
	public:
		int		LoadConfig(){return 0;}

		int		ReloadConfig(){return 0;}

		//��ȡ��־����
		int		LoadLogConfig(){return 0;}

	}; //class CNetConfig
}

//ȫ������
#define CONF (CSingleton<CNetConfig, 0>::Instance())
#endif