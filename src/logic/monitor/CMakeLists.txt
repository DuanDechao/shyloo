INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(MONITOR
	"${PROJECT_SOURCE_DIR}/src/logic/monitor/Monitor.h"
	"${PROJECT_SOURCE_DIR}/src/logic/monitor/Monitor.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/monitor/ShutDown.h"
	"${PROJECT_SOURCE_DIR}/src/logic/monitor/ShutDown.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/monitor/ServerState.h"
	"${PROJECT_SOURCE_DIR}/src/logic/monitor/ServerState.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/monitor/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${MONITOR})

ADD_LIBRARY(Monitor SHARED
	${MONITOR}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(Monitor slxml.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET Monitor PROPERTY FOLDER "core")