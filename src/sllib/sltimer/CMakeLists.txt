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


SET(SL_LIB_TIMER
	"${PROJECT_SOURCE_DIR}/src/sllib/sltimer/sltime_stamp.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/sltimer/sltime_stamp.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/sltimer/sltimer_base.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/sltimer/sltimer_base.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/sltimer/sltimer1.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/sltimer/sltimer1.inl"
	"${PROJECT_SOURCE_DIR}/src/sllib/sltimer/sltimer1.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/sltimer/sltimer_mgr.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/sltimer/sltimer_mgr.cpp"
)

SOURCE_GROUP(api FILES ${SL_LIB_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${SL_LIB_TIMER})

ADD_LIBRARY(sltimer STATIC
	${SL_LIB_API}
	${PUBLIC}
	${SL_LIB_TIMER}
)

SET_PROPERTY(TARGET sltimer PROPERTY FOLDER "sllib")