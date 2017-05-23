#ifndef __EVENT_ID_H__
#define __EVENT_ID_H__
#include "slmulti_sys.h"
class IObject;
namespace logic_event{
	enum{
		EVENT_LOGIC_PLAYER_RECONNECT = 1,
		EVENT_LOGIC_PLAYER_ONLINE,
		EVENT_LOGIC_PLAYER_FIRST_ONLINE,
		EVENT_LOGIC_PLAYER_LOST,
		EVENT_LOGIC_PLAYER_DESTROY,
		EVENT_LOGIC_DATA_LOAD_COMPLETED,
		EVENT_LOGIC_PREPARE_APPEAR_SCENE,
		EVENT_LOGIC_APPEAR_ON_SCENE,
		EVENT_LOGIC_PREPARE_ENTER_SCENE,
		EVENT_LOGIC_ENTER_SCENE,
		EVENT_LOGIC_PREPARE_LEAVE_SCENE,
		EVENT_LOGIC_LEAVE_SCENE,
		EVENT_LOGIC_PLAYER_APPEAR,
		EVENT_LOGIC_SCENE_OBJECT_APPEAR,
		EVENT_LOGIC_SCENE_OBJECT_DESTROY,
		EVENT_LOGIC_SHADOW_CREATED,
		EVENT_LOGIC_ENTER_VISION,
		EVENT_LOGIC_LEAVE_VISION,
		EVENT_LOGIC_SEE_SOMEONE,
		EVENT_LOGIC_MISS_SOMEONE,

		EVENT_SCENE_ENTER_SCENE = 1000,
		EVENT_SCENE_APPEAR_SCENE,
		EVENT_SCENE_LEAVE_SCENE,

		EVENT_DB_UPDATE_FINISHED = 2000,
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

	struct VisionInfo{
		IObject* object;
		int64 watcherId;
		int32 gate;
	};

	struct WatchInfo{
		IObject* object;
		int64 interester;
		int32 type;
	};
}
#endif