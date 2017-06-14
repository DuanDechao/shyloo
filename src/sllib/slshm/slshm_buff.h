///�����ڴ���Ϊ����Buffʱ�Ĺ�����������
/********************************************************************
	created:	2015/12/15
	created:	15:12:2015   20:11
	filename: 	d:\workspace\shyloo\sllib\ipc\slshm_buff.h
	file path:	d:\workspace\shyloo\sllib\ipc
	file base:	slshm_buff
	file ext:	h
	author:		ddc
	
	purpose:	�����ڴ���Ϊ����Buffʱ�Ĺ�����������
*********************************************************************/

#ifndef __SL_LIB_SHM_BUFF_H__
#define __SL_LIB_SHM_BUFF_H__
#include "slshm.h"

namespace sl{
	enum enumShm{
		ES_BUFFNAME_MAX		=	24,			//�����ڴ�����
		ES_BUFFSIZE_MAX		=	1000,		//����ڴ��������
	};

	typedef int(*PInitBuffFunc)(char* pstBuff, unsigned int uiBuffSize, bool bResetShm);

	//�����ڴ�Buff�ڵ���
	class CShmBuffNode{
	private:
		char			m_szBuffName[ES_BUFFNAME_MAX + 1];		///< buff����
		unsigned int	m_uiBuffSize;							///< Buff��С
		PInitBuffFunc   m_stPInitFunc;							///< �����ڴ洴������
	
	public:
		CShmBuffNode(){
			m_szBuffName[0] = '\0';
			m_uiBuffSize    = 0;
			m_stPInitFunc   = NULL;
		}

		CShmBuffNode(const char* pszName,
			unsigned int uiSize,
			PInitBuffFunc stFunc)
			:m_uiBuffSize(uiSize), m_stPInitFunc(stFunc)
		{
			SafeSprintf(m_szBuffName, ES_BUFFNAME_MAX, "%s", pszName);
			ECHO_TRACE("alloc buff(%s) node name (%s)", pszName, m_szBuffName);
		}

		const char* GetBuffName() const{
			return m_szBuffName;
		}

		unsigned int GetBuffSize() const{
			return m_uiBuffSize;
		}

		int MakeBuff(char* pszBuff, bool bResetShm){
			if(m_stPInitFunc == NULL){
				SLASSERT(false,"shm buff(%s) do size(%d) do not have init Func!", m_szBuffName, m_uiBuffSize);
				return -1;
			}

			return (*m_stPInitFunc)(pszBuff, m_uiBuffSize, bResetShm);
		}

		CShmBuffNode& operator = (const CShmBuffNode& obj){
			SafeSprintf(m_szBuffName, ES_BUFFNAME_MAX, "%s", obj.m_szBuffName);
			m_uiBuffSize = obj.m_uiBuffSize;
			m_stPInitFunc = obj.m_stPInitFunc;
			return *this;
		}
	};

	//�����ڴ�Buff������
	class CShmBuff{
	private:
		CShmBuffNode	m_stShmArray[ES_BUFFSIZE_MAX];			///< �����ڴ������б�
		unsigned int	m_uiShmSize;							///<
		SLShm			m_stShm;								///< �����ڴ����ݶ���
		bool			m_bIsResetShm;							///< �Ƿ���Ҫ���³�ʼ������
		uint64			m_ui64BuffSize;							///< �ܵĹ����ڴ�����

		int InsertBuff(const CShmBuffNode& stNode){
			if(m_uiShmSize >= ES_BUFFSIZE_MAX){
				//�����ڴ�buff����
				SLASSERT(false, "shm buff list is full!");
				return 1;
			}
			m_stShmArray[m_uiShmSize] = stNode;
			++m_uiShmSize;
			return 0;
		}

		int BuffAttachShm(){
			//�������ڲ������������ȥ�ж��Ƿ��Ѿ���������
			char* pstBuff = m_stShm.GetBuffer();
			for(unsigned int i = 0; i < m_uiShmSize; ++i){
				int iRet = m_stShmArray[i].MakeBuff(pstBuff, m_bIsResetShm);
				if(iRet){
					SLASSERT("make buff(%s) err(%d)!", m_stShmArray[i].GetBuffName(), iRet);
					return iRet;
				}
				ECHO_TRACE("alloc shm Buff(%s) at addr(%p) size(%u)",
					m_stShmArray[i].GetBuffName(),
					pstBuff,
					m_stShmArray[i].GetBuffSize());
				pstBuff += m_stShmArray[i].GetBuffSize();
			}
			return 0;
		}
	
	public:
		CShmBuff(){
			m_uiShmSize		=	0;
			m_ui64BuffSize	=	0;
			m_bIsResetShm	=	true;
		}

		virtual ~CShmBuff(){}

		void SetResetShmFlag(bool bIsResetshm){
			m_bIsResetShm = bIsResetshm;
		}
		
		/*
			ע���ڴ滺����
			@param [in]	stBuffName	����������
			@param [in] uiBuffSize	��������С
			@param [in] pFunc		ע���ʼ������
		*/
		int RegisterBuff(const char* stBuffName, unsigned int uiBuffSize, PInitBuffFunc pFunc){
			if(pFunc == NULL){
				SLASSERT(false, "register buff(%s) is no init func!", stBuffName);
				return -1;
			}

			CShmBuffNode stNewNode(stBuffName, uiBuffSize, pFunc);
			int iRet = InsertBuff(stNewNode);
			if(iRet){
				SLASSERT(false, "shm buff list is full");
				return -1;
			}

			m_ui64BuffSize = m_ui64BuffSize + uiBuffSize;
			if((size_t)m_ui64BuffSize < 0){
				//�����ڴ��Ѿ�Խ��
				SLASSERT(false, "need size(%lu) is over load!", m_ui64BuffSize);
				return -1;
			}
			ECHO_TRACE("buff(%s) size(%u) register into shm size count(%lu)", stBuffName, uiBuffSize, m_ui64BuffSize);
			return 0;
		}
#ifndef SL_OS_WINDOWS
		int CreateBuff(key_t uikey){
			int iRet = m_stShm.Create(uikey, (size_t)m_ui64BuffSize);
			if(iRet){
				SLASSERT(false, "make shm fails(%d)!", iRet);
				return iRet;
			}

			iRet = BuffAttachShm();
			if(iRet){
				SLASSERT(false, "buff attach shm fails(%d)!", iRet);
			}
			return iRet;
		}
#endif

		int CreateBuff(const char* pszPathName)		//���������ڴ�buff
		{
			///�ȴ��������ڴ�
			int iRet = m_stShm.Create(pszPathName, (size_t)m_ui64BuffSize);
			if(iRet){	
				SLASSERT(false, "make shm fails(%d)!", iRet);
				return iRet;
			}
			
			iRet = BuffAttachShm();
			if(iRet){
				SLASSERT(false, "buff attach shm fails(%d)!", iRet);
			}
			return iRet;

		}
	};
}
#endif
