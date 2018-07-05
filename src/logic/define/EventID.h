#ifndef __EVENT_ID_H__
#define __EVENT_ID_H__
#include "slmulti_sys.h"
class IObject;
namespace logic_event{
	enum{
		EVENT_SERVER_READY = 1,
		EVENT_PRE_SHUTDOWN,
		EVENT_SHUTDOWN_NOTIFY,
		EVENT_SHUTDOWN_CLOSE,
		EVENT_SHUTDOWN_COMPLETE,

		EVENT_NEW_MINUTE,
		EVENT_NEW_HOUR,
		EVENT_NEW_DAY,
		EVENT_NEW_WEEK,
		EVENT_NEW_MONTH,
		EVENT_DAY_CHANGED,
		EVENT_WEEK_CHANGED,
		EVENT_MONTH_CHANGED,

        EVENT_NEW_OBJECT_CREATED,
        EVENT_CREATE_CELL_ENTITY,
        EVENT_CELL_ENTITY_CREATED,
		EVENT_PROXY_CREATED,
        
        EVENT_GATE_LOGINED,
        EVENT_PROXY_ENTITY_CREATED,
        EVENT_ENTITY_CREATED_FROM_DB_CALLBACK,

		EVENT_LOGIC_PLAYER_RECONNECT = 1000,
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
		EVENT_LOGIC_PLAYER_GATE_LOST,
		EVENT_LOGIC_CREATE_BASE_ANYWHERE,
		EVENT_LOGIC_CREATE_BASE_ANYWHERE_CALLBACK,

		EVENT_SCENE_ENTER_SCENE = 2000,
		EVENT_SCENE_APPEAR_SCENE,
		EVENT_SCENE_LEAVE_SCENE,

		EVENT_DB_UPDATE_FINISHED = 3000,

		EVENT_RELATION_SYNC_CHAT_CHANNEL_ADDRESS = 4000,
	};
	
	struct Biology{
		IObject* object;
	};

    struct CellEntityCreated{
        IObject* object;
		const void* cellData;
		const int32 cellDataSize;
		const void* initData;
		const int32 initDataSize;
        int32 baseNodeId;
    };

    struct CellEntityCreatedFromCell{
        uint64 entityId;
        int32 remoteNodeId;
    };

    struct GateLoginedInfo{
        const char* proxyType;
        uint64 proxyId;
        int64 agentId;
    };

    struct ProxyEntityCreated{
        uint64 proxyId;
        int64 agentId;
    };

    struct EntityCreatedFromDBCallBack{
        const char* entityType;
        uint64 dbid;
        uint64 callbackId;
        uint64 entityId;
        bool success;
        bool wasActive;        
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

	struct ShutDown{
		int8 step;
	};

	struct PreShutDown{};

	struct NewMinute{
		int64 tick;
		int32 time;
	};

	struct NewHour{
		int64 tick;
		int32 time;
	};

	struct NewDay{
		int64 tick;
		int32 time;
	};

	struct NewWeek{
		int32 week;
	};

	struct NewMonth{
		int32 month;
	};

	struct ChatAddress{
		const char* ip;
		int32 port;
	};

	struct StartUp{
	};

	struct ProxyCreated{
		IObject* object;
		int64 agentId;
	};
	
	struct CreateBaseAnywhere{
		IObject* object;
		const void* initData;
		const int32 dataSize;
	};

	struct CreateBaseAnywhereCallback{
		uint64 callbackId;
		const char* entityType;
		uint64 entityId;
		int32 createNodeId;
	}; 
}
#endif
