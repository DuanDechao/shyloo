INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(LOGIC
	"${PROJECT_SOURCE_DIR}/src/logic/logic/Logic.h"
	"${PROJECT_SOURCE_DIR}/src/logic/logic/Logic.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/logic/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${LOGIC})

ADD_LIBRARY(Logic SHARED
	${LOGIC}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(Logic AttrGetter.lib slxml.lib)
ENDIF(WIN32)

#IF(UNIX)
#TARGET_LINK_LIBRARIES(Logic
# debug "${PROJECT_SOURCE_DIR}/build/linux/libAttrGetter.so"
# optimized "${PROJECT_SOURCE_DIR}/build/linux/libAttrGetter.so"
#)
#ENDIF(UNIX)

#SET_PROPERTY(TARGET Gate PROPERTY FOLDER "core")
SET_PROPERTY(TARGET Logic PROPERTY FOLDER "framework")