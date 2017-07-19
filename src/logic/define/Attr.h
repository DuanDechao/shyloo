#ifndef __ATTR_H__
#define __ATTR_H__
#include "slmulti_sys.h" 

#ifdef SL_OS_WINDOWS
#ifdef ATTR_EXPORT
#define ATTR_API __declspec (dllexport)
#else
#define ATTR_API __declspec (dllimport)
#endif
#else
#define ATTR_API
#endif

class IProp;

namespace prop_def {
	enum {
		visible = 1,
		share = 2,
		save = 4,
		significant = 8,
		blob = 16,
		copy = 32,
	};
}

struct ATTR_API attr_def{
	static const IProp* account;
	static const IProp* agent;
	static const IProp* ai;
	static const IProp* aiInterval;
	static const IProp* appear;
	static const IProp* exp;
	static const IProp* firstLogin;
	static const IProp* gate;
	static const IProp* hp;
	static const IProp* id;
	static const IProp* logic;
	static const IProp* maxHp;
	static const IProp* maxMp;
	static const IProp* mp;
	static const IProp* name;
	static const IProp* occupation;
	static const IProp* offlineTime;
	static const IProp* oldX;
	static const IProp* oldY;
	static const IProp* oldZ;
	static const IProp* recoverTimer;
	static const IProp* scene;
	static const IProp* sceneId;
	static const IProp* sex;
	static const IProp* startTime;
	static const IProp* state;
	static const IProp* status;
	static const IProp* type;
	static const IProp* vision;
	static const IProp* x;
	static const IProp* y;
	static const IProp* z;
};

struct ATTR_API OCTempProp{
	static const IProp* AITIMER;
	static const IProp* AI_REF;
	static const IProp* DAY_CHANGE_TIMER;
	static const IProp* IS_APPEAR;
	static const IProp* MONTH_CHANGE_TIMER;
	static const IProp* PROP_UPDATE_TIMER;
	static const IProp* RGS_SHADOW;
	static const IProp* SCENE_X_NODE;
	static const IProp* SCENE_Y_NODE;
	static const IProp* SCENE_Z_NODE;
	static const IProp* SHADOW_SYNC_TIMER;
	static const IProp* SYNCTOSCENE;
	static const IProp* SYNCTOSCENE_TIMER;
	static const IProp* WEEK_CHANGE_TIMER;
};

namespace OCTableMacro {
	namespace AITABLE {
		static int32 TABLE_NAME = 1700400206;
		enum {
			OCTM_START = 0,
			STATE = OCTM_START,
			PARAM1,
			PARAM2,
			PARAM3,
			PARAM4,
			PARAM5,
			OCTM_END,
		};
	}

	namespace CHECK_TIMER {
		static int32 TABLE_NAME = -1844346954;
		enum {
			OCTM_START = 0,
			TIMER = OCTM_START,
			OCTM_END,
		};
	}

	namespace AOI_WATCHERS {
		static int32 TABLE_NAME = 1615486319;
		enum {
			OCTM_START = 0,
			ID = OCTM_START,
			LOGIC,
			GATE,
			OCTM_END,
		};
	}

	namespace PROP_SELF_TABLE {
		static int32 TABLE_NAME = 2026016795;
		enum {
			OCTM_START = 0,
			PROP = OCTM_START,
			OCTM_END,
		};
	}

	namespace PROP_SHARED_TABLE {
		static int32 TABLE_NAME = -1172264420;
		enum {
			OCTM_START = 0,
			PROP = OCTM_START,
			OCTM_END,
		};
	}

	namespace SCENEOBJECTS {
		static int32 TABLE_NAME = 112589896;
		enum {
			OCTM_START = 0,
			ID = OCTM_START,
			OCTM_END,
		};
	}

	namespace AOI_INTERESTERS {
		static int32 TABLE_NAME = 1168952880;
		enum {
			OCTM_START = 0,
			ID = OCTM_START,
			TYPE,
			OCTM_END,
		};
	}

	namespace SHADOW_SYNC_PROP {
		static int32 TABLE_NAME = -1881068084;
		enum {
			OCTM_START = 0,
			PROP = OCTM_START,
			OCTM_END,
		};
	}

	namespace SHADOW {
		static int32 TABLE_NAME = -2071500652;
		enum {
			OCTM_START = 0,
			LOGIC = OCTM_START,
			COUNT,
			OCTM_END,
		};
	}

};

namespace OCStaticTableMacro {
	namespace STATICSCENEOBJECTS {
		static const char* TABLE_NAME = "STATICSCENEOBJECTS";
		enum {
			OCTM_START = 0,
			ID = OCTM_START,
			SCENEID,
			TYPE,
			OCTM_END,
		};
	}

	namespace SCENES {
		static const char* TABLE_NAME = "SCENES";
		enum {
			OCTM_START = 0,
			ID = OCTM_START,
			NAME,
			NODE,
			STATE,
			INDEX,
			COUNT,
			OCTM_END,
		};
	}

	namespace SCENECOPY {
		static const char* TABLE_NAME = "SCENECOPY";
		enum {
			OCTM_START = 0,
			ID = OCTM_START,
			OBJECT,
			OCTM_END,
		};
	}

};

#endif
