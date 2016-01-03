#include "../sllib/slmsg_base.h"
#include "../slproject_common/agame_msgid.h"
#include "../sllib/slcode_convert.h"
using namespace sl;

//心跳 处理随时间自动更改的数据
class CReqHeartbeat: public CReqMsgBase<MSGID_ACCOUNT_HEART_BEAT>
{};
BEGIN_CODE_CONVERT(CReqHeartbeat)
END_CODE_CONVERT(CReqHeartbeat)

class CAnsHeartbeat: public CAnsMsgBase<MSGID_ACCOUNT_HEART_BEAT>
{
public:
	int			m_iNow;		//當前時間
};
BEGIN_CODE_CONVERT(CAnsHeartbeat)
	CODE_CONVERT(m_iResult)
	CODE_CONVERT(m_iNow)
END_CODE_CONVERT(CAnsHeartbeat)
