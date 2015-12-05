///client套接字对象工厂类
/********************************************************************
	created:	2015/12/02
	created:	2:12:2015   17:30
	filename: 	e:\myproject\shyloo\netsvr\netclientfactory.h
	file path:	e:\myproject\shyloo\netsvr
	file base:	netclientfactory
	file ext:	h
	author:		ddc
	
	purpose:	client套接字对象工厂类
*********************************************************************/

#ifndef _NET_CLIENT_FACTORY_H_
#define _NET_CLIENT_FACTORY_H_
#include "netdef.h"
#include "netclient.h"

namespace sl
{
	enum
	{
		LIST_COUNT	=	2,
		INDEX_COUNT	=	2,

		//第一条索引表示是否分配
		ALLOC_INDEX	=	0,
		FREE_LIST	=	0,
		USED_LIST	=	1,

		//第二条索引表示是否有残留数据
		REMAIN_INDEX	=	1,
		EMPTY_LIST		=	0,
		REMAIN_LIST		=	1,
	};

	class CNetClientFactory
	{
	protected:
		CObjectMgr<CNetClient>			m_stMgr;
		unsigned int					m_uiClientSeq;			///< 客户端序号，每个客户端连接分配一个

	public:
		CNetClientFactory() {}
		virtual ~CNetClientFactory(){}

		int Init(const char* pszBuffer, size_t iBufferSize, int iObjectCount);

		static size_t CountSize(int iObjectCount);

		/*
			根据下标返回Client对象
			@return 参数非法时返回NULL
		*/
		CNetClient* CalcObject(int iIndex);

		//pstClient 在数组中的下标
		int CalcObjectPos(const CNetClient* pstClient) const;

		CNetClient* Alloc();
		int Free(CNetClient* pstClient);

		int Head(int iListID, int iIndexID)
		{
			return m_stMgr.Head(iListID, iIndexID);
		}

		int Next(int i, int iIndexID)
		{
			return m_stMgr.Next(i, iIndexID);
		}

		int Size(int iListID, int iIndexID)
		{
			return m_stMgr.Size(iListID, iIndexID);
		}

		int GetObjectCount() const
		{
			return m_stMgr.GetObjectCount();
		}

		int SetEmpty(CNetClient* pstClient);
		int SetRemain(CNetClient* pstClient);
		bool IsEmpty(CNetClient* pstClient);
		int IsFree(CNetClient* pstClient)
		{
			int i = CalcObjectPos(pstClient);
			return m_stMgr.Flag(i, ALLOC_INDEX) == FREE_LIST;
		}

		unsigned int GetClientSeq()
		{
			return ++m_uiClientSeq;
		}




	};
}
#endif