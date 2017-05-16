#ifndef __EVENT_ID_H__
#define __EVENT_ID_H__
#include "slmulti_sys.h"
class IObject;
namespace logic_event{
	enum{
		EVENT_GATE_RECONNECT = 1,
		EVENT_PLAYER_ONLINE = 2,
		EVENT_GATE_LOST = 3,
		EVENT_PLAYER_DESTROY =4,
		EVENT_DB_UPDATE_FINISHED = 5,
		EVENT_DATA_LOAD_COMPLETED = 6,
	};
	
	struct Biology{
		IObject* object;
	};

	struct DBUpdateFinished{

	};
}
#endif