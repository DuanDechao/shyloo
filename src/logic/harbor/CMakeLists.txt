INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(HARBOR
	"${PROJECT_SOURCE_DIR}/src/logic/harbor/Harbor.h"
	"${PROJECT_SOURCE_DIR}/src/logic/harbor/Harbor.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/harbor/NodeSession.h"
	"${PROJECT_SOURCE_DIR}/src/logic/harbor/NodeSession.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/harbor/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${HARBOR})

ADD_LIBRARY(Harbor SHARED
	${HARBOR}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

TARGET_LINK_LIBRARIES(Harbor
	${SL_LIB_DEBUG}
	${SL_LIB_RELEASE}
)

SET_PROPERTY(TARGET Harbor PROPERTY FOLDER "core")