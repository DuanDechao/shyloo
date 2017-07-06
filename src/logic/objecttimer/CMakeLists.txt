INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(OBJECTTIMER
	"${PROJECT_SOURCE_DIR}/src/logic/objecttimer/OCTimer.h"
	"${PROJECT_SOURCE_DIR}/src/logic/objecttimer/OCTimer.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/objecttimer/ObjectTimer.h"
	"${PROJECT_SOURCE_DIR}/src/logic/objecttimer/ObjectTimer.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/objecttimer/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${OBJECTTIMER})

ADD_LIBRARY(ObjectTimer SHARED
	${OBJECTTIMER}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

SET_PROPERTY(TARGET ObjectTimer PROPERTY FOLDER "framework")