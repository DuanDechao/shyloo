cmake_minimum_required(VERSION 2.8)

IF(WIN32)
	INCLUDE_DIRECTORIES(
		"${PROJECT_SOURCE_DIR}/src/public"
		"${PROJECT_SOURCE_DIR}/src/sllib/api"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32"
	)
	
	SET(HIREDIS
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/async.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/async.c"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/dict.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/dict.c"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/fmacros.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/hiredis.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/hiredis.c"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/net.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/net.c"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/read.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/read.c"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/sds.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/sds.c"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/config.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis_win32/win32.h"
	)
	
ENDIF(WIN32)

IF(UNIX)
	INCLUDE_DIRECTORIES(
		"${PROJECT_SOURCE_DIR}/src/public"
		"${PROJECT_SOURCE_DIR}/src/sllib/api"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis"
	)
	
	SET(HIREDIS
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/dict.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/dict.c"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/fmacros.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/hiredis.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/hiredis.c"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/net.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/net.c"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/read.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/read.c"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/sds.h"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/sds.c"
		"${PROJECT_SOURCE_DIR}/src/sllib/slredis/hiredis/sdsalloc.h"
	)
ENDIF(UNIX)

IF(WIN32)
	SET(SYS_LIB_DEBUG
		debug ""
	)
ENDIF(WIN32)

SET(SL_LIB_REDIS
	"${PROJECT_SOURCE_DIR}/src/sllib/slredis/slredis_callresult.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slredis/slredis_callresult.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slredis/slredis_connection.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slredis/slredis_connection.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slredis/slredis_mgr.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slredis/slredis_mgr.cpp"
)

SOURCE_GROUP(api FILES ${SL_LIB_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(hiredis FILES ${HIREDIS})
SOURCE_GROUP(\\ FILES ${SL_LIB_REDIS})

ADD_LIBRARY(slredis SHARED
	${SL_LIB_API}
	${PUBLIC}
	${HIREDIS}
	${SL_LIB_REDIS}
)
SET_PROPERTY(TARGET slredis PROPERTY FOLDER "sllib")