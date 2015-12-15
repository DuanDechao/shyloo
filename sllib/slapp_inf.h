//appsvr�ṩ��ҵ���߼�so��ʹ�õĽӿ���
/********************************************************************
	created:	2015/12/15
	created:	15:12:2015   20:50
	filename: 	d:\workspace\shyloo\sllib\slapp_inf.h
	file path:	d:\workspace\shyloo\sllib
	file base:	slapp_inf
	file ext:	h
	author:		ddc
	
	purpose:	appsvr�ṩ��ҵ���߼�so��ʹ�õĽӿ���
*********************************************************************/
#ifndef __SL_APP_INF_H__
#define __SL_APP_INF_H__
#include "slassert.h"
#include "slasync_cmd_factory.h"
#include "slmsg_base.h"
#include "ipc/slshm_buff.h"

namespace sl
{
	///һ����ʼΪNULL��ָ��
	namespace detail
	{
		class CVoidPtr
		{
		public:
			CVoidPtr(): p(NULL){}
			void* p;
		};
	}

	//AppSo�ĵ���
	#define SL_APP_RAW	(CSingleton<detail::CVoidPtr, 0>::Instance()->p)
	#define SL_APP ((CAppInf*)SL_APP_RAW)

	//���ߵ�ԭ��
	enum EnumUserDisconnectReason
	{
		EUDR_UNKNOW		=	0,		///< ����ԭ����Ҫ�Ƿ�����ԭ��
		EUDR_KICK		=	1,		///< ���ߵ����ظ���¼
		EUDR_SVR_HALT	=	2,		///< Svrͣ��
		EUDR_MULT_LOGIN =   3,		///< ��ε�¼ ���Ѿ���¼��������ַ����˵�¼Э��
		EUDR_LOGIN_FAIL =	4,		///< ��¼ʧ��
		EUDR_FROZEEN	=	5,		///< ���
		EUDR_OFFLINE	=	6,		///< �û�����
		EUSR_RECONN		=	7,		///< ��������ʱҪ����ǰ��NetHead�����
	};

	//AppSvr�������̳У�Ϊso�ṩ����AppSvr������
	class CAppInf
	{
	public:
		CAppInf()
		{
			//��CAppSvr��ʼ��ʱ����CAppSvr���󸳸�SL_APP
			SL_APP_RAW = this;
		}
		virtual ~CAppInf() {}

		/*
			�ڴ�Ԥ����
			@param [in] szBuffName   buffer����
			@param [in] uiBuffSize	 buffer��С
			@param [in] pInitFunc	 ��ʼ��buffer�ĺ���
		*/
		virtual int RegisterBuffer(const char* szBuffName, unsigned int uiBuffSize, PInitBuffFunc pInitFunc) = 0;
		
		//��ȡ��ǰ̨����Ϣ�ı��뻺����
		virtual CBuffer& GetClientEncodeBuffer() = 0;
		virtual int SendData(unsigned int uiDPKey, CNetHead& stHead, const char* pszBuf, int iBufLen) = 0;

		///��ȡ������svr����Ϣ�ı��뻺����
		virtual CBuffer& GetServerEncodeBuffer() = 0;
		virtual int SendToServer(int iSvrID, const char* pszBuf, int iBufLen) = 0;

		// ��CNetHead���ж��û��Ƿ��¼
		virtual bool IsLogined(const CNetHead& stHead) = 0;

		//��ʱ��д��2������
		virtual void AttachUserToWB(sl::uid_t iUID) = 0;
		virtual void DettachUserFromWB(sl::uid_t iUID) = 0;
	};
}
#endif