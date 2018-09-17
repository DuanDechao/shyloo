INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(MASTER
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/master/Master.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/master/Master.cpp"
)

SET(MONITOR
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/monitor/Monitor.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/monitor/Monitor.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/monitor/StartUpHandler.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/monitor/StartUpHandler.cpp"
)
SET(SLAVE
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/slave/Slave.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/slave/Slave.cpp"
)

SET(STARTER
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/starter/Starter.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/starter/Starter.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/starter/StartNodeTimer.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/starter/StartNodeTimer.cpp"
)

SET(CLUSTER
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/Cluster.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/Cluster.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(master FILES ${MASTER})
SOURCE_GROUP(monitor FILES ${MONITOR})
SOURCE_GROUP(slave FILES ${SLAVE})
SOURCE_GROUP(starter FILES ${STARTER})
SOURCE_GROUP(\\ FILES ${CLUSTER})

ADD_LIBRARY(Cluster SHARED
	${MASTER}
	${MONITOR}
	${SLAVE}
	${STARTER}
	${CLUSTER}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
	TARGET_LINK_LIBRARIES(Cluster slxml.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET Cluster PROPERTY FOLDER "core")
