#ifndef __EVENT_ID_H__
#define __EVENT_ID_H__
#include "slmulti_sys.h"
class IObject;
namespace logic_event{
	enum{
		EVENT_LOGIC_PLAYER_RECONNECT = 1,
		EVENT_LOGIC_PLAYER_ONLINE = 2,
		EVENT_LOGIC_PLAYER_LOST = 3,
		EVENT_LOGIC_PLAYER_DESTROY =4,
		EVENT_DB_UPDATE_FINISHED = 5,
		EVENT_LOGIC_DATA_LOAD_COMPLETED = 6,
		EVENT_LOGIC_PREPARE_APPEAR_SCENE = 7,
		EVENT_LOGIC_APPEAR_ON_SCENE = 8,
		EVENT_LOGIC_PREPARE_ENTER_SCENE = 9,
		EVENT_LOGIC_ENTER_SCENE = 10,
		EVENT_LOGIC_PREPARE_LEAVE_SCENE = 11,
		EVENT_LOGIC_LEAVE_SCENE = 12,
		EVENT_LOGIC_PLAYER_APPEAR = 13,
		EVENT_SCENE_ENTER_SCENE = 14,
		EVENT_SCENE_APPEAR_SCENE = 15,
		EVENT_SCENE_LEAVE_SCENE = 16,
	};
	
	struct Biology{
		IObject* object;
	};

	struct DBUpdateFinished{

	};

	struct EnterVision{
		IObject* object;
		int64	id;
	};

	struct AppearVision{
		IObject* object;
		int64 id;
	};

	struct LeaveVision{
		IObject* object;
		int64 id;
	};
}
#endif