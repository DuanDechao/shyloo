cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/thirdlibrary/mysql/x64/include"
)

IF(WIN32)
	SET(SYS_LIB_DEBUG
		debug ""
	)
ENDIF(WIN32)


SET(SL_LIB_DB
	"${PROJECT_SOURCE_DIR}/src/sllib/sldb/sldb_define.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/sldb/sldb_result.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/sldb/sldb_result.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/sldb/sldb_connection.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/sldb/sldb_connection.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/sldb/sldb_connection_pool.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/sldb/sldb_connection_pool.cpp"
	#"${PROJECT_SOURCE_DIR}/src/sllib/sldb/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_LIB_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${SL_LIB_DB})

ADD_LIBRARY(sldb SHARED
	${SL_LIB_API}
	${PUBLIC}
	${SL_LIB_DB}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(sldb
	"${PROJECT_SOURCE_DIR}/thirdlibrary/mysql/x64/lib/libmysql.lib"
)
ENDIF(WIN32)

SET_PROPERTY(TARGET sldb PROPERTY FOLDER "sllib")