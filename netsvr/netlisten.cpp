/********************************************************************
	created:	2015/12/02
	created:	2:12:2015   16:57
	filename: 	e:\myproject\shyloo\netsvr\netlisten.cpp
	file path:	e:\myproject\shyloo\netsvr
	file base:	netlisten
	file ext:	cpp
	author:		ddc
	
	purpose:	
*********************************************************************/

#include "netclient.h"
#include "netctrl.h"

using namespace sl;

int CNetListen::Init(CNetCtrl& stOwner, const CNetListenInfo& stListenInfo)
{
	int iRet = 0;

	m_pstOwner		=	&stOwner;
	m_stListenInfo	=	stListenInfo;
	
	iRet	=	m_stSocket.Listen(m_stListenInfo.m_szListenIP(), m_stListenInfo.m_unListenPort);
	CHECK_RETURN(iRet);
	
	iRet	=	Register(stOwner,
		&CNetCtrl::OnListenEvent,
		stOwner.m_stEpoll,
		m_stSocket.GetSocket(),
		EPOLLIN);

	CHECK_RETURN(iRet);
	return 0;
}