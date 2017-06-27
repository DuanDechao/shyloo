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


SET(SL_LIB_SHM
	#"${PROJECT_SOURCE_DIR}/src/sllib/slshm/slshm_create.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slshm/slshm_buff.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slshm/slshm_queue.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slshm/slshm_queue.cpp"
	#"${PROJECT_SOURCE_DIR}/src/sllib/slshm/slcode_queue.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/slshm/slshm_mgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/sllib/slshm/slshm_mgr.h"
	#"${PROJECT_SOURCE_DIR}/src/sllib/sltimer/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_LIB_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(\\ FILES ${SL_LIB_SHM})

ADD_LIBRARY(slshm SHARED
	${SL_LIB_API}
	${PUBLIC}
	${SL_LIB_SHM}
)

SET_PROPERTY(TARGET slshm PROPERTY FOLDER "sllib")