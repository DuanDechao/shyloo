INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(DCCENTER
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/ObjectStruct.h"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/ObjectStruct.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/Memory.h"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/ObjectProp.h"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/ObjectProp.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/MMObject.h"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/MMObject.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/ObjectMgr.h"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/ObjectMgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/TableRow.h"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/TableRow.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/TableControl.h"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/TableControl.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/ObjectFSM.h"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/ObjectFSM.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/dccenter/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${DCCENTER})

ADD_LIBRARY(DCCenter SHARED
	${DCCENTER}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(DCCenter slxml.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET DCCenter PROPERTY FOLDER "core")