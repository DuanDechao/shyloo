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


SET(SL_LIB_PYSCRIPT
	"${PROJECT_SOURCE_DIR}/src/sllib/slpyscript/slpymacros.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slpyscript/slscript_object.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slpyscript/slscript_object.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slpyscript/slpyscript.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slpyscript/slpyscript.cpp"
)

SOURCE_GROUP(api FILES ${SL_LIB_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${SL_LIB_PYSCRIPT})

ADD_LIBRARY(slpyscript STATIC
	${SL_LIB_API}
	${PUBLIC}
	${SL_LIB_PYSCRIPT}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(slpyscript python34_d.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET slpyscript PROPERTY FOLDER "sllib")