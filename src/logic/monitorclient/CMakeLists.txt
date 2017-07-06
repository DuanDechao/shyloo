INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(MONITORCLIENT
	"${PROJECT_SOURCE_DIR}/src/logic/monitorclient/MonitorClient.h"
	"${PROJECT_SOURCE_DIR}/src/logic/monitorclient/MonitorClient.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/monitorclient/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${MONITORCLIENT})

ADD_LIBRARY(MonitorClient SHARED
	${MONITORCLIENT}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

SET_PROPERTY(TARGET MonitorClient PROPERTY FOLDER "core")