INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(STARTER
	"${PROJECT_SOURCE_DIR}/src/logic/starter/Starter.h"
	"${PROJECT_SOURCE_DIR}/src/logic/starter/Starter.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/starter/StartNodeTimer.h"
	"${PROJECT_SOURCE_DIR}/src/logic/starter/StartNodeTimer.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/starter/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${STARTER})

ADD_LIBRARY(Starter SHARED
	${STARTER}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(Starter slxml.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET Starter PROPERTY FOLDER "core")