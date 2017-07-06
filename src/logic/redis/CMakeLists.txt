INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(REDIS
	"${PROJECT_SOURCE_DIR}/src/logic/redis/Redis.h"
	"${PROJECT_SOURCE_DIR}/src/logic/redis/Redis.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/redis/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${REDIS})

ADD_LIBRARY(Redis SHARED
	${REDIS}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(Redis slredis.lib slxml.lib)
ENDIF(WIN32)

#IF(UNIX)
#TARGET_LINK_LIBRARIES(Redis
#    debug "${PROJECT_SOURCE_DIR}/build/linux/libslredis.so"
#	optimized "${PROJECT_SOURCE_DIR}/build/linux/libslredis.so"
#)
#ENDIF(UNIX)

SET_PROPERTY(TARGET Redis PROPERTY FOLDER "core")
