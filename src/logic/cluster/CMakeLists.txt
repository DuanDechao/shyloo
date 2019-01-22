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
SET(SLAVE
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/slave/Slave.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/slave/Slave.cpp"
)
SET(NODE
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/node/Node.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/node/Node.cpp"
)
SET(MACHINE
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/machine/Machine.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/machine/Machine.cpp"
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


IF(UNIX)
SET(SIGAR
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/linux_sigar.c"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar.c"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_fileinfo.c"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_format.c"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_getline.c"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_os.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_ptql.c"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_signal.c"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_util.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_cache.c"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_fileinfo.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_format.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_getline.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_log.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_private.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_ptql.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_util.c"
	"${PROJECT_SOURCE_DIR}/src/logic/cluster/sigar/linux/sigar_version.c.in"
)
ENDIF(UNIX)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(sigar FILES ${SIGAR})
SOURCE_GROUP(master FILES ${MASTER})
SOURCE_GROUP(slave FILES ${SLAVE})
SOURCE_GROUP(machine FILES ${MACHINE})
SOURCE_GROUP(starter FILES ${STARTER})
SOURCE_GROUP(node FILES ${NODE})
SOURCE_GROUP(\\ FILES ${CLUSTER})

ADD_LIBRARY(Cluster SHARED
	${SIGAR}
	${MASTER}
	${SLAVE}
	${STARTER}
	${CLUSTER}
	${NODE}
	${MACHINE}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
	TARGET_LINK_LIBRARIES(Cluster slxml.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET Cluster PROPERTY FOLDER "core")
