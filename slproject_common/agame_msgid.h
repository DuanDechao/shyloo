#ifndef __AGAME_MSGID_H__
#define __AGAME_MSGID_H__
#include "../sllib/slmsg_base.h"

namespace sl
{
	///��ϢID���x
	enum
	{
		MSGID_APPSVR_START				=	MSGID_LOGIN_END + 5,		///< 1010:Appsvr����ʼ��Ϣ
		MSGID_ACCOUNT_HEART_BEAT		=	MSGID_APPSVR_START	+ 17,	///< 1027��������ϢID
	};
}
#endif