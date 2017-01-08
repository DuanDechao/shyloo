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


SET(SL_LIB_FILE
	"${PROJECT_SOURCE_DIR}/src/sllib/slfile/slfile_base.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slfile/slfile_base.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slfile/slfile_mgr.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slfile/slfile_mgr.cpp"
)

SOURCE_GROUP(api FILES ${SL_LIB_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${SL_LIB_FILE})

ADD_LIBRARY(slfile STATIC
	${SL_LIB_API}
	${PUBLIC}
	${SL_LIB_FILE}
)
SET_PROPERTY(TARGET slfile PROPERTY FOLDER "sllib")