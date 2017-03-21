INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(DISTRIBUTION
	"${PROJECT_SOURCE_DIR}/src/logic/distribution/Distribution.h"
	"${PROJECT_SOURCE_DIR}/src/logic/distribution/Distribution.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/distribution/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${DISTRIBUTION})

ADD_LIBRARY(Distribution SHARED
	${DISTRIBUTION}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

TARGET_LINK_LIBRARIES(Distribution
	${SL_LIB_DEBUG}
	${SL_LIB_RELEASE}
)

SET_PROPERTY(TARGET Distribution PROPERTY FOLDER "framework")
#SET_PROPERTY(TARGET EventEngine PROPERTY FOLDER "framework")