
IF(WIN32)
INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/kernel/interface"
	"${PROJECT_SOURCE_DIR}/src/kernel"
	"${PROJECT_SOURCE_DIR}/src/kernel/logic"
	"${PROJECT_SOURCE_DIR}/src/kernel/timer"
	"${PROJECT_SOURCE_DIR}/src/kernel/net"
	"${PROJECT_SOURCE_DIR}/src/kernel/config"
	"${PROJECT_SOURCE_DIR}/src/kernel/db"
)
ENDIF(WIN32)

SET(SL_KERNEL_INTERFACE
	"${PROJECT_SOURCE_DIR}/src/kernel/interface/slicore.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/interface/slinet_engine.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/interface/slitimer_engine.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/interface/slilogic_engine.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/interface/sliconfig_engine.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/interface/sliapplication.h"
)

SET(SL_KERNEL_NET
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_engine.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_engine.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_session.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_session.cpp"
)

SET(SL_KERNEL_TIMER
	"${PROJECT_SOURCE_DIR}/src/kernel/timer/slkr_timer.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/timer/slkr_timer.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/timer/sltimer_engine.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/timer/sltimer_engine.cpp"
)

SET(SL_KERNEL_LOGIC
	"${PROJECT_SOURCE_DIR}/src/kernel/logic/sllogic_engine.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/logic/sllogic_engine.cpp"
)

SET(SL_KERNEL_CONFIG
	"${PROJECT_SOURCE_DIR}/src/kernel/config/slconfig_engine.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/config/slconfig_engine.cpp"
)

SET(SL_KERNEL_DB
	"${PROJECT_SOURCE_DIR}/src/kernel/db/sldb_task.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/db/sldb_task.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/db/sldb_engine.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/db/sldb_engine.cpp"
)

SET(SL_KERNEL
	"${PROJECT_SOURCE_DIR}/src/kernel/slkernel.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/slkernel.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/slmain.cpp"
)
SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(src\\interface FILES ${SL_KERNEL_INTERFACE})
SOURCE_GROUP(src\\net FILES ${SL_KERNEL_NET})
SOURCE_GROUP(src\\timer FILES ${SL_KERNEL_TIMER})
SOURCE_GROUP(src\\logic FILES ${SL_KERNEL_LOGIC})
SOURCE_GROUP(src\\config FILES ${SL_KERNEL_CONFIG})
SOURCE_GROUP(src\\db FILES ${SL_KERNEL_DB})
SOURCE_GROUP(\\ FILES ${SL_KERNEL})

IF(WIN32)
ADD_EXECUTABLE(shyloo
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_KERNEL_INTERFACE}
	${SL_KERNEL_NET}
	${SL_KERNEL_TIMER}
	${SL_KERNEL_LOGIC}
	${SL_KERNEL_CONFIG}
	${SL_KERNEL_DB}
	${SL_KERNEL}
)
ENDIF(WIN32)
TARGET_LINK_LIBRARIES(shyloo
	${SL_LIB_DEBUG}
	${SL_LIB_RELEASE}
)
IF(WIN32)
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/windows")
ENDIF(WIN32)
