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


SET(SL_LIB_XML
	"${PROJECT_SOURCE_DIR}/src/sllib/slxml/tinyxml.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slxml/tinystr.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slxml/tinystr.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slxml/tinyxml.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slxml/tinyxmlerror.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slxml/tinyxmlparser.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slxml/xml.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slxml/xml.cpp"
)

SOURCE_GROUP(api FILES ${SL_LIB_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${SL_LIB_XML})

ADD_LIBRARY(slxml SHARED
	${SL_LIB_API}
	${PUBLIC}
	${SL_LIB_XML}
)
SET_PROPERTY(TARGET slxml PROPERTY FOLDER "sllib")