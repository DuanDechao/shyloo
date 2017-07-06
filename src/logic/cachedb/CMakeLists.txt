INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(CACHEDB
	"${PROJECT_SOURCE_DIR}/src/logic/cachedb/CacheDBStruct.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cachedb/CacheDBStruct.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/cachedb/CacheDB.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cachedb/CacheDB.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/cachedb/DataLand.h"
	"${PROJECT_SOURCE_DIR}/src/logic/cachedb/DataLand.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/cachedb/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${CACHEDB})

ADD_LIBRARY(CacheDB SHARED
	${CACHEDB}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(CacheDB slxml.lib slredis.lib)
ENDIF(WIN32)

#IF(UNIX)
#TARGET_LINK_LIBRARIES(CacheDB
#    debug "${PROJECT_SOURCE_DIR}/build/linux/libslredis.so"
#    optimized "${PROJECT_SOURCE_DIR}/build/linux/libslredis.so"
#)
#ENDIF(UNIX)

SET_PROPERTY(TARGET CacheDB PROPERTY FOLDER "core")
