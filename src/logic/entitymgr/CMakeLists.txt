INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/src/sllib/slpyscript"
)

SET(ENTITYMGR
	"${PROJECT_SOURCE_DIR}/src/logic/entitymgr/EntityMgr.h"
	"${PROJECT_SOURCE_DIR}/src/logic/entitymgr/EntityMgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/entitymgr/Base.h"
	"${PROJECT_SOURCE_DIR}/src/logic/entitymgr/Base.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/entitymgr/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${ENTITYMGR})

ADD_LIBRARY(EntityMgr SHARED
	${ENTITYMGR}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(EntityMgr slxml.lib shyloo.lib slpyscript.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET EntityMgr PROPERTY FOLDER "logic")