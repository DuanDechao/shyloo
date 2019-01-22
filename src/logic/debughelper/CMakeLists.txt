INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	#	"${PROJECT_SOURCE_DIR}/thirdlibrary/log4cxx/include"
)

SET(DEBUGHELPER
	"${PROJECT_SOURCE_DIR}/src/logic/debughelper/DebugHelper.h"
	"${PROJECT_SOURCE_DIR}/src/logic/debughelper/DebugHelper.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/debughelper/main.cpp"
)

#SET(LOGGER
#	"${PROJECT_SOURCE_DIR}/src/logic/debughelper/logger/Logger.h"
#	"${PROJECT_SOURCE_DIR}/src/logic/debughelper/logger/Logger.cpp#"
#	"${PROJECT_SOURCE_DIR}/src/logic/debughelper/logger/LogSyncHandler.h"
#	"${PROJECT_SOURCE_DIR}/src/logic/debughelper/logger/LogSyncHandler.cpp"
#)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
#SOURCE_GROUP(logger FILES ${LOGGER})
SOURCE_GROUP(\\ FILES ${DEBUGHELPER})

ADD_LIBRARY(DebugHelper SHARED
	${DEBUGHELPER}
	#	${LOGGER}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(DebugHelper slxml.lib)
ENDIF(WIN32)

IF(UNIX)
TARGET_LINK_LIBRARIES(DebugHelper)
ENDIF(UNIX)

SET_PROPERTY(TARGET DebugHelper PROPERTY FOLDER "core")
