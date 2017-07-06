INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(MYSQLMGR
	"${PROJECT_SOURCE_DIR}/src/logic/mysqlmgr/SQLBuilder.h"
	"${PROJECT_SOURCE_DIR}/src/logic/mysqlmgr/SQLBuilder.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/mysqlmgr/MysqlBase.h"
	"${PROJECT_SOURCE_DIR}/src/logic/mysqlmgr/MysqlBase.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/mysqlmgr/MysqlMgr.h"
	"${PROJECT_SOURCE_DIR}/src/logic/mysqlmgr/MysqlMgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/mysqlmgr/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${MYSQLMGR})

ADD_LIBRARY(MysqlMgr SHARED
	${MYSQLMGR}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(MysqlMgr sldb.lib slxml.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET MysqlMgr PROPERTY FOLDER "core")