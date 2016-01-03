#include "../sllib/slmsg_base.h"
#include "../slproject_common/agame_msgid.h"
#include "../sllib/slcode_convert.h"
using namespace sl;

//���� ������ʱ���Զ����ĵ�����
class CReqHeartbeat: public CReqMsgBase<MSGID_ACCOUNT_HEART_BEAT>
{};
BEGIN_CODE_CONVERT(CReqHeartbeat)
END_CODE_CONVERT(CReqHeartbeat)

class CAnsHeartbeat: public CAnsMsgBase<MSGID_ACCOUNT_HEART_BEAT>
{
public:
	int			m_iNow;		//��ǰ�r�g
};
BEGIN_CODE_CONVERT(CAnsHeartbeat)
	CODE_CONVERT(m_iResult)
	CODE_CONVERT(m_iNow)
END_CODE_CONVERT(CAnsHeartbeat)
