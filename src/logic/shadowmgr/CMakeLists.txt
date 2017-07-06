INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(SHADOWMGR
	"${PROJECT_SOURCE_DIR}/src/logic/shadowmgr/ShadowMgr.h"
	"${PROJECT_SOURCE_DIR}/src/logic/shadowmgr/ShadowMgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/shadowmgr/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${SHADOWMGR})

ADD_LIBRARY(ShadowMgr SHARED
	${SHADOWMGR}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(ShadowMgr AttrGetter.lib slxml.lib)
ENDIF(WIN32)

#IF(UNIX)
#TARGET_LINK_LIBRARIES(ShadowMgr
# debug "${PROJECT_SOURCE_DIR}/build/linux/libAttrGetter.so"
# optimized "${PROJECT_SOURCE_DIR}/build/linux/libAttrGetter.so"
#)
#ENDIF(UNIX)


#SET_PROPERTY(TARGET Gate PROPERTY FOLDER "core")
SET_PROPERTY(TARGET ShadowMgr PROPERTY FOLDER "framework")
