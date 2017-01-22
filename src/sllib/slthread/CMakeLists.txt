cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

IF(WIN32)
	SET(SYS_LIB_DEBUG
		debug ""
	)
ENDIF(WIN32)


SET(SL_LIB_THREAD
	"${PROJECT_SOURCE_DIR}/src/sllib/slthread/sltpthread.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slthread/sltpthread.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slthread/slthread_pool.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slthread/slthread_pool.cpp"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slthread/test.cpp"
)

SOURCE_GROUP(api FILES ${SL_LIB_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${SL_LIB_THREAD})

ADD_LIBRARY(slthread STATIC
	${SL_LIB_API}
	${PUBLIC}
	${SL_LIB_THREAD}
)
SET_PROPERTY(TARGET slthread PROPERTY FOLDER "sllib")