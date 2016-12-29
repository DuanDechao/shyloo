INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(STARTER
	"${PROJECT_SOURCE_DIR}/src/logic/starter/Starter.h"
	"${PROJECT_SOURCE_DIR}/src/logic/starter/Starter.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/starter/StartNodeTimer.h"
	"${PROJECT_SOURCE_DIR}/src/logic/starter/StartNodeTimer.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${STARTER})

ADD_LIBRARY(starter SHARED
	${STARTER}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
	${SL_LIB_API}
)

SET_PROPERTY(TARGET starter PROPERTY FOLDER "core")