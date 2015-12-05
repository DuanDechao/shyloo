/********************************************************************
	created:	2015/12/02
	created:	2:12:2015   20:29
	filename: 	e:\myproject\shyloo\netsvr\netclientfactory.cpp
	file path:	e:\myproject\shyloo\netsvr
	file base:	netclientfactory
	file ext:	cpp
	author:		ddc
	
	purpose:	
*********************************************************************/

#include "netclientfactory.h"

using namespace sl;

int CNetClientFactory::Init(const char* pszBuffer, size_t iBufferSize, int iObjectCount)
{
	SL_ASSERT(CountSize(iObjectCount) == iBufferSize);

	int iRet = 0;
	iRet = m_stMgr.Init(pszBuffer, iObjectCount, sizeof(CNetClient), LIST_COUNT,
		INDEX_COUNT);
	CHECK_RETURN(iRet);

	return 0;
}

size_t CNetClientFactory::CountSize(int iObjectCount)
{
	return CObjectMgr<CNetClient>::CountSize(iObjectCount,
		sizeof(CNetClient), LIST_COUNT, INDEX_COUNT);
}

CNetClient* CNetClientFactory::Alloc()
{
	int i = m_stMgr.InfoListAlloc(ALLOC_INDEX, USED_LIST);
	if(i < 0)
	{
		SL_WARNING("alloc client failed %d",i);
		return NULL;
	}

	CNetClient* pstClient = new (&m_stMgr[i]) CNetClient;
	pstClient->m_stNetHead.m_Handle	 = i;

	return pstClient;
}

int CNetClientFactory::Free(CNetClient* pstClient)
{
	int i = CalcObjectPos(pstClient);

	m_stMgr.InfoListMove(i, REMAIN_INDEX, EMPTY_LIST);
	m_stMgr.InfoListMove(i, ALLOC_INDEX, FREE_LIST);

	pstClient->~CNetClient();

	return 0;
}

CNetClient* CNetClientFactory::CalcObject(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_stMgr.GetObjectCount())
	{
		return NULL;
	}
	return &m_stMgr[iIndex];
}

int CNetClientFactory::CalcObjectPos(const CNetClient* pstClient) const
{
	int i = (char*)pstClient - (char*)&m_stMgr[0];
	SL_ASSERT(i % sizeof(CNetClient) == 0);
	return i / sizeof(CNetClient);
}

int CNetClientFactory::SetEmpty(CNetClient* pstClient)
{
	int i = CalcObjectPos(pstClient);

	switch(m_stMgr.Flag(i, REMAIN_INDEX))
	{
	case EMPTY_LIST:
		return 0;
	case REMAIN_LIST:
		return m_stMgr.InfoListMove(i, REMAIN_INDEX, EMPTY_LIST);
	default:
		SL_WARNING("client(%d) invalid listid %d", i, m_stMgr.Flag(i, REMAIN_INDEX));
		return -1;
	}
}

int CNetClientFactory::SetRemain(CNetClient* pstClient)
{
	int i = CalcObjectPos(pstClient);

	switch(m_stMgr.Flag(i, REMAIN_INDEX))
	{
	case EMPTY_LIST:
		return m_stMgr.InfoListMove(i, REMAIN_INDEX, REMAIN_LIST);
	case REMAIN_LIST:
		return 0;
	default:
		SL_WARNING("client(%d) invalid listid %d", i, m_stMgr.Flag(i, REMAIN_INDEX));
		return -1;
	}
}

bool CNetClientFactory::IsEmpty(CNetClient* pstClient)
{
	int i = CalcObjectPos(pstClient);
	return m_stMgr.Flag(i, REMAIN_INDEX) == EMPTY_LIST;
}




