#ifndef __EVENT_ID_H__
#define __EVENT_ID_H__
#include "slmulti_sys.h"
class IObject;
namespace EventID{
	enum{
		EVENT_GATE_RECONNECT = 1,
		EVENT_PLAYER_ONLINE = 2,
		EVENT_GATE_LOST = 3,
		EVENT_PLAYER_DESTROY =4,
	};
	
	struct Biology{
		IObject* object;
	};
}
#endif