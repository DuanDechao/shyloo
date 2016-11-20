#include "slfixed_messages.h"
namespace sl
{
SL_SINGLETON_INIT(network::FixedMessages);

namespace network
{
FixedMessages::FixedMessages()
	:m_infomap(),
	 m_loaded(false)
{
	//new Re
}
}
}