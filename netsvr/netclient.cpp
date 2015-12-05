/********************************************************************
	created:	2015/12/02
	created:	2:12:2015   16:24
	filename: 	e:\myproject\shyloo\netsvr\netclient.cpp
	file path:	e:\myproject\shyloo\netsvr
	file base:	netclient
	file ext:	cpp
	author:		ddc
	
	purpose:	
*********************************************************************/

#include "netclient.h"
#include "netctrl.h"
using namespace sl;

int CNetClient::Init(CNetCtrl& stOwner, CNetListen& stListen, 
					 SOCKET iSocket, unsigned int uiIP, unsigned int ushPort)
{
	SL_ASSERT(iSocket != SL_INVALID_SOCKET);

	m_pstOwner	=	&stOwner;
	m_pstListen	=	&stListen;
	m_stSocket.SetSocket(iSocket);

	//ע�ᵽepoll
	int iRet = Register(
		stOwner,
		&CNetCtrl::OnClientEvent,
		stOwner.m_stEpoll,
		m_stSocket.GetSocket(),
		EPOLLIN);
		CHECK_RETURN(iRet);
	const CNetListenInfo& stListenInfo = m_pstListen->GetListenInfo();
	m_stNetHead.m_LocalIP		=	inet_addr(stListenInfo.m_szListenIP.Get());
	m_stNetHead.m_LocalPort		=	stListenInfo.m_unListenPort;
	m_stNetHead.m_RemoteIP		=	uiIP;
	m_stNetHead.m_RemotePort	=	ushPort;

	m_stNetHead.m_LastTime		=	stOwner.m_uiNow;
	m_stNetHead.m_CreateTime	=	stOwner.m_uiNow;
	m_stNetHead.m_LastTime		=	m_stNetHead.m_CreateTime;
	m_stNetHead.m_Act1			=	0;
	m_stNetHead.m_Act2			=	0;
	m_stNetHead.m_LiveFlag		=	0;
	m_stNetHead.m_EncodeMethod	=	stListenInfo.m_ucEncodeMethod;
	m_stNetHead.m_DecodeMethod	=	stListenInfo.m_ucDecodeMethod;
	m_stNetHead.m_Key[0]		=	'\0';

	return 0;
}