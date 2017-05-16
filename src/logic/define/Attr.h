#ifndef __ATTR_H__
#define __ATTR_H__

#ifdef ATTR_EXPORT
#define ATTR_API __declspec (dllexport)
#else
#define ATTR_API __declspec (dllimport)
#endif

class IProp;

namespace prop_def {
	enum {
		visible = 1,
		share = 2,
		save = 4,
		significant = 8,
		blob = 16,
	};
}

struct ATTR_API attr_def{
	static const IProp* account;
	static const IProp* ai;
	static const IProp* aiInterval;
	static const IProp* exp;
	static const IProp* gate;
	static const IProp* id;
	static const IProp* logic;
	static const IProp* name;
	static const IProp* occupation;
	static const IProp* recoverTimer;
	static const IProp* sex;
	static const IProp* status;
};

struct ATTR_API OCTempProp{
	static const IProp* AITIMER;
	static const IProp* AI_REF;
	static const IProp* PROP_UPDATE_TIMER;
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

};

#endif
