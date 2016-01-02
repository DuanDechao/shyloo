//�༶ObjectMgr��ÿһ���Ķ����С����
/********************************************************************
	created:	2015/12/05
	created:	5:12:2015   15:56
	filename: 	e:\myproject\shyloo\sllib\slmulti_object_mgr.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slmulti_object_mgr
	file ext:	h
	author:		ddc
	
	purpose:	�༶ObjectMgr
*********************************************************************/
#ifndef _SL_MULTI_OBJECT_MGR_H_
#define _SL_MULTI_OBJECT_MGR_H_
#include "slbase.h"
#include "slobject_mgr.h"
#include "slobj_reload_check.h"
namespace sl
{
	//�༶ObjectMgr���±���Ϣ
	class CMgrIndex
	{
	public:
		int		iMgrIndex;			///< ��CMultiObjectMgr�е��ĸ�Mgr
		int		iObjIndex;			///< ��Mgr�е��±�

		CMgrIndex(): iMgrIndex(-1), iObjIndex(-1) {}
		
		CMgrIndex(const CMgrIndex& obj)
		{
			iMgrIndex = obj.iMgrIndex;
			iObjIndex = obj.iObjIndex; 
		}

		CMgrIndex& operator = (const CMgrIndex& obj)
		{
			iMgrIndex = obj.iMgrIndex;
			iObjIndex = obj.iObjIndex;
			return *this;
		}

		bool operator == (const CMgrIndex& obj)
		{
			return iMgrIndex == obj.iMgrIndex &&
					iObjIndex == obj.iObjIndex;
		}
		bool operator != (const CMgrIndex& obj)
		{
			return ! operator == (obj);
		}

		void Clear()
		{
			iMgrIndex = -1;
			iObjIndex = -1;
		}

		bool IsNull() const
		{
			return (iMgrIndex == -1 || iObjIndex == -1);
		}
	}; /// class CMgrIndex

	class CMgrConfig
	{
	public:
		int		m_iObjectCount;
		int		m_iObjectSize;

		bool operator < (const CMgrConfig& obj) const
		{
			return m_iObjectSize < obj.m_iObjectSize;
		}
	}; // class CMgrConfig

	///�༶��ObjectMgr
	class CMultiObjectMgr
	{
	protected:
		typedef CObjectMgr<char*> TObjectMgr;
		typedef vector<TObjectMgr> TObjectMgrVec;

		typedef vector<CMgrConfig> TMgrConfigVec;
		TObjectMgrVec		m_astObjectMgr;
	
	public:
		CMultiObjectMgr() {}
		virtual ~CMultiObjectMgr() {}

		/*
			��ʼ��
			@param [in]  pszMgrConfig		��ͬ��С���ڴ�������
				�ַ�����ʽ��(�ڴ���С1,�ڴ������)...(�ڴ���Сn,�ڴ������n)
			@param [in]  pszBuffer   	�ڴ��׵�ַ(�ڴ����ⲿ�����)
			@param [in]  iBufferSize	�ڴ�Ĵ�С������Ҫ����CountSize()��������Ĵ�С
		*/
		int Init(const char* pszMgrConfig, const char* pszBuffer, int iBufferSize,
			int iListCount = 2, int iIndexCount = 1, bool bReload = true)
		{
			int iRet = 0;
			TMgrConfigVec ast;
			iRet = BuildMgrConfigVec(pszMgrConfig, ast);
			if(iRet)
			{
				return iRet;
			}
			return Init(ast, pszBuffer, iBufferSize, iListCount, iIndexCount, bReload);
		}


		/*
			����astMgrConfig��Ҫ���ڴ���С�����������ڴ�/�����ڴ�
			@param [in]  pszMgrConfig		��ͬ��С���ڴ�������
			@return ���������ڴ���ܴ�С����λ���ֽ�
		*/
		static size_t CountSize(const char* pszMgrConfig, int iListCount = 2, int iIndexCount = 1)
		{
			int iRet = 0;
			TMgrConfigVec ast;
			iRet  = BuildMgrConfigVec(pszMgrConfig, ast);
			if(iRet)
			{
				return iRet;
			}
			return CountSize(ast, iListCount, iIndexCount);
		}

		/*
			���Ҫ���仺������Ӧ�ô��ĸ�Mgr�з���
			���㷽���Ǵӿ�ʼ�������ҵ����������С��Mgr
			@return ����Mgr���±꣬�����������ͷ���-1
		*/
		int CalcMgrIndex(int iSize) const
		{
			for (size_t i = 0; i < m_astObjectMgr.size(); ++i)
			{
				if(m_astObjectMgr[i].GetObjectSize() >= iSize)
				{
					return static_cast<int>(i);
				}
			}
			return -1;
		}

		/*
			���仺����
			��Ҫ������placement new
			@param [in]  iSize		Ҫ����Ļ�������С
			@param [out] pstIndex	���仺�������±���Ϣ
		*/
		char* Alloc(int iMgrIndex, CMgrIndex* pstIndex)
		{
			if(pstIndex)
			{
				pstIndex->Clear();
			}
			if (iMgrIndex < 0)
			{
				return NULL;
			}

			//����ռ�
			TObjectMgr& stMgr = m_astObjectMgr[iMgrIndex];
			int iIndex = stMgr.InfoListAlloc(0, 1);
			if(iIndex < 0)
			{
				SL_WARNING("Mgr[%d].Alloc failed %d", iMgrIndex, iIndex);
				return NULL;
			}
			if(pstIndex)
			{
				pstIndex->iMgrIndex = iMgrIndex;
				pstIndex->iObjIndex = iIndex;
			}
			return m_astObjectMgr[iMgrIndex][iIndex];
		}

		/*
			�жϻ������ǲ��Ǵ���������
			@param [in]   pOject	Ҫ���Ļ�����
			@param [out]  pstIndex	���滺�������±���Ϣ
		*/
		bool IsValidObject(const char* pObject, CMgrIndex* pstIndex) const
		{
			if( !pObject)
			{
				return false;
			}
			for (size_t i = 0; i < m_astObjectMgr.size(); i++)
			{
				const TObjectMgr& stMgr = m_astObjectMgr[i];
				if (pObject >= stMgr[0] && pObject <= stMgr[stMgr.GetObjectCount() - 1])
				{
					if( (pObject - stMgr[0])  % stMgr.GetObjectSize() == 0 )
					{
						if(pstIndex)
						{
							pstIndex->iMgrIndex = static_cast<int>(i);
							pstIndex->iObjIndex = static_cast<int>( (pObject - stMgr[0]) / stMgr.GetObjectSize() );
						}
						return true;
					}
					else
					{
						return false;
					}
				}
			}
			return false;
		}


		/*
			�ж��±���Ϣ�ǲ��Ǵ���������
			@param [in]   stIndex	Ҫ�����±���Ϣ
		*/
		bool IsValidIndex(const CMgrIndex& stIndex) const
		{
			if(stIndex.iMgrIndex < 0 || stIndex.iMgrIndex >= (int)m_astObjectMgr.size() ||
				stIndex.iObjIndex < 0 || stIndex.iObjIndex >= (int)m_astObjectMgr[stIndex.iMgrIndex].GetObjectCount())
			{
				return false;
			}
			return true;
		}

		/*
			�ͷŻ�����
			��Ҫ���������placement new�����Ķ������������
		*/
		int Free(const char* pObject)
		{
			CMgrIndex stIndex;
			if(!IsValidObject(pObject, &stIndex))
			{
				return -1;
			}
			return Free(stIndex);
		}

		/*
			�ͷŻ�����
			�Է��仺����ʱ���±���Ϣ������Ч�ʸ���
		*/
		int Free(const CMgrIndex& stIndex)
		{
			if(!IsValidIndex(stIndex))
			{
				return -1;
			}
			m_astObjectMgr[stIndex.iMgrIndex].InfoListMove(stIndex.iObjIndex, 0, 0);
			return 0;
		}


		//�����±���Ϣ��Ӧ�Ļ�����
		char* Get(const CMgrIndex& stIndex)
		{
			return m_astObjectMgr[stIndex.iMgrIndex][stIndex.iObjIndex];
		}
		size_t GetMgrCount() const
		{
			return m_astObjectMgr.size();
		}
		int GetMgrObjectSize(int iMgrIndex) const
		{
			return m_astObjectMgr[iMgrIndex].GetObjectSize();
		}

		int GetMgrObjectCount(int iMgrIndex) const
		{
			return m_astObjectMgr[iMgrIndex].GetObjectCount();
		}

		int Size(int iMgrIndex, int iIndexID, int iListID)
		{
			return m_astObjectMgr[iMgrIndex].Size(iListID, iIndexID);
		}

		//���ĳһ��MgrIndex������iIndexID���Ǹ�����
		int Flag(CMgrIndex& stIndex, int iIndexID)
		{
			if(stIndex.IsNull())
			{
				return -1;
			}
			return m_astObjectMgr[stIndex.iMgrIndex].Flag(stIndex.iObjIndex, iIndexID);
		}

		/*
			��ö༶ObjectMgrĳ�������ͷ��
			@param [out] stIndex		ͷ�����±���Ϣ���������Ϊ�գ���stIndex.IsNull
		*/
		void GetHead(CMgrIndex& stIndex, int iListID, int iIndexID) const
		{
			stIndex.Clear();
			for (size_t i = 0; i< m_astObjectMgr.size(); ++i)
			{
				stIndex.iMgrIndex = static_cast<int>(i);
				stIndex.iObjIndex = m_astObjectMgr[i].Head(iListID, iIndexID);
				if(!stIndex.IsNull())
				{
					return;
				}
			}
		}

		//��ȡ�༶ObjectMgrĳ���������һ�����
		void GetNext(const CMgrIndex& stIndex, CMgrIndex& stNext, int iIndexID) const
		{
			stNext.iMgrIndex  = stIndex.iMgrIndex;
			stNext.iObjIndex  = m_astObjectMgr[stIndex.iMgrIndex].Next(stIndex.iObjIndex, iIndexID);
			if(!stNext.IsNull())
			{
				return;
			}

			//����������ObjectMgr�Ҳ�����һ������ˣ�������һ�������ObjectMgr��
			int iListID = m_astObjectMgr[stIndex.iMgrIndex].Flag(stIndex.iObjIndex, iIndexID);
			for(size_t i = stIndex.iMgrIndex + 1; i < m_astObjectMgr.size(); ++i)
			{
				stNext.iMgrIndex  =  static_cast<int>(i);
				stNext.iObjIndex  = m_astObjectMgr[i].Head(iListID, iIndexID);
				if(!stNext.IsNull())
				{
					return;
				}
			}
		}

		//�ѽ���ƶ�����һ������
		int Move(const CMgrIndex& stIndex, int iIndexID, int iNewListID)
		{
			TObjectMgr& stMgr = m_astObjectMgr[stIndex.iMgrIndex];
			return stMgr.InfoListMove(stIndex.iObjIndex, iIndexID, iNewListID);
		}

	protected:
		/*
			��ʼ��
			@param [in]  astMgrConfig	��ͬ��С���ڴ�������
			@param [in]  pszBuffer		�ڴ��׵�ַ(�ڴ����ⲿ�����)
			@param [in]  iBufferSize	�ڴ�Ĵ�С������Ҫ����CountSize()��������Ĵ�С
		*/
		int Init(const TMgrConfigVec& astMgrConfig, const char* pszBuffer,
			size_t iBufferSize, int iListCount = 2, int iIndexCount = 1, bool bResetShm = true)
		{
			const size_t iAllSize = CountSize(astMgrConfig, iListCount, iIndexCount);
			if(iAllSize != iBufferSize || iAllSize <= 0)
			{
				return -1;
			}
			size_t iPos = 0;
			int iRet  = 0;
			for (size_t i = 0; i < astMgrConfig.size(); ++i)
			{
				m_astObjectMgr.push_back(TObjectMgr());
				iRet  = m_astObjectMgr[i].Init(pszBuffer + iPos, astMgrConfig[i].m_iObjectCount,
					astMgrConfig[i].m_iObjectSize, iListCount, iIndexCount, bResetShm);
				if(iRet)
				{
					return iRet;
				}

				iPos += TObjectMgr::CountSize(astMgrConfig[i].m_iObjectCount, astMgrConfig[i].m_iObjectSize,
					iListCount, iIndexCount);
			}
			return 0;
		}

		/*
			����astMgrConfig��Ҫ���ڴ���С�����������ڴ�/�����ڴ�
			@param [in]  astMgrConfig		��ͬ��С���ڴ�������
			@return ����������ڴ���ܴ�С�� ��λ�ֽ�
		*/
		static size_t CountSize(const TMgrConfigVec& astMgrConfig, int iListCount = 2, int iIndexCount = 1)
		{
			int iAllSize = 0;
			for (size_t i = 0; i < astMgrConfig.size(); ++i)
			{
				iAllSize += static_cast<int>(TObjectMgr::CountSize(astMgrConfig[i].m_iObjectCount, 
					astMgrConfig[i].m_iObjectSize, iListCount,iIndexCount));
			}
			return iAllSize;
		}
	public:
		/*
			���ַ���������astMgrConfig
			�ַ�����ʽ��(�ڴ���С1���ڴ������1)...(�ڴ���С2, �ڴ������2)...(�ڴ���Сn,�ڴ������n)
			�ڴ���С �ĵ�λ��byte
		*/
		static int BuildMgrConfigVec(const char* pszMgrConfig, TMgrConfigVec& astMgrConfig)
		{
			astMgrConfig.clear();
			
			string s(pszMgrConfig);
			CStringUtils::RemoveNot(s, "01234567890,)");
			
			vector<string> astKeyValue;
			CStringUtils::Split(s, ")", astKeyValue);
			
			string sSize, sCount;
			for (size_t i = 0; i < astKeyValue.size(); ++i)
			{
				if(!CStringUtils::SplitIni(astKeyValue[i], sSize, sCount, ','))
				{
					astMgrConfig.clear();
					return -1;
				}
				CMgrConfig stConfig;
				stConfig.m_iObjectSize   =   CStringUtils::StrToInt<int>(sSize.c_str());
				stConfig.m_iObjectCount  =   CStringUtils::StrToInt<int>(sCount.c_str());
				if( !(stConfig.m_iObjectSize > 0 && stConfig.m_iObjectCount > 0))
				{
					astMgrConfig.clear();
					return -2;
				}
				astMgrConfig.push_back(stConfig);
			}

			//����
			sort(astMgrConfig.begin(), astMgrConfig.end());
			return 0;
		}
	};


} //namespace sl
#endif