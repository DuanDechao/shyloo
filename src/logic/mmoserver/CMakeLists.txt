INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(MMOSERVER
    "${PROJECT_SOURCE_DIR}/src/logic/mmoserver/BaseApp.h"
    "${PROJECT_SOURCE_DIR}/src/logic/mmoserver/BaseApp.cpp"
    "${PROJECT_SOURCE_DIR}/src/logic/mmoserver/CellApp.h"
    "${PROJECT_SOURCE_DIR}/src/logic/mmoserver/CellApp.cpp"
    "${PROJECT_SOURCE_DIR}/src/logic/mmoserver/DBMgr.h"
    "${PROJECT_SOURCE_DIR}/src/logic/mmoserver/DBMgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/mmoserver/CellAppMgr.h"
	"${PROJECT_SOURCE_DIR}/src/logic/mmoserver/CellAppMgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/mmoserver/BaseAppMgr.h"
	"${PROJECT_SOURCE_DIR}/src/logic/mmoserver/BaseAppMgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/mmoserver/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${MMOSERVER})

ADD_LIBRARY(MmoServer SHARED
    ${MMOSERVER}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(MmoServer AttrGetter.lib slxml.lib)
ENDIF(WIN32)

IF(UNIX)
TARGET_LINK_LIBRARIES(MmoServer libAttrGetter.so)
ENDIF(UNIX)

#SET_PROPERTY(TARGET Gate PROPERTY FOLDER "core")
SET_PROPERTY(TARGET MmoServer PROPERTY FOLDER "framework")
