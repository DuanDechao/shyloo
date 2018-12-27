INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/thirdlibrary/mysql/x64/include"
)

SET(DATABASE
	"${PROJECT_SOURCE_DIR}/src/logic/db/DBContext.h"
	"${PROJECT_SOURCE_DIR}/src/logic/db/DB.h"
	"${PROJECT_SOURCE_DIR}/src/logic/db/DB.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/db/DBTable.h"
	"${PROJECT_SOURCE_DIR}/src/logic/db/DBTable.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/db/DBTableMysql.h"
	"${PROJECT_SOURCE_DIR}/src/logic/db/DBTableMysql.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/db/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${DATABASE})

ADD_LIBRARY(DB SHARED
	${DATABASE}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

SET_PROPERTY(TARGET DB PROPERTY FOLDER "core")
