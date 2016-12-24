INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
)

SET(HARBOR
	"${PROJECT_SOURCE_DIR}/src/logic/harbor/Harbor.h"
	"${PROJECT_SOURCE_DIR}/src/logic/harbor/Harbor.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/harbor/NodeSession.h"
	"${PROJECT_SOURCE_DIR}/src/logic/harbor/NodeSession.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${HARBOR})

ADD_LIBRARY(harbor SHARED
	${HARBOR}
	${SL_KERNEL_API}
	${PUBLIC}
)

SET_PROPERTY(TARGET harbor PROPERTY FOLDER "core")