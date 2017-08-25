INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(STARTNODESTRATEGY
	"${PROJECT_SOURCE_DIR}/src/logic/startnodestrategy/StartNodeStrategy.h"
	"${PROJECT_SOURCE_DIR}/src/logic/startnodestrategy/StartNodeStrategy.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/startnodestrategy/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${STARTNODESTRATEGY})

ADD_LIBRARY(StartNodeStrategy SHARED
	${STARTNODESTRATEGY}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(StartNodeStrategy slxml.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET StartNodeStrategy PROPERTY FOLDER "framework")