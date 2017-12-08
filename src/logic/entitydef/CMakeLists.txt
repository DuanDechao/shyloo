INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/src/sllib/slpyscript"
)

SET(ENTITYDEF
	"${PROJECT_SOURCE_DIR}/src/logic/entitydef/DataType.h"
	"${PROJECT_SOURCE_DIR}/src/logic/entitydef/DataType.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/entitydef/DataTypeMgr.h"
	"${PROJECT_SOURCE_DIR}/src/logic/entitydef/DataTypeMgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/entitydef/EntityDef.h"
	"${PROJECT_SOURCE_DIR}/src/logic/entitydef/EntityDef.cpp"
	#"${PROJECT_SOURCE_DIR}/src/logic/entitydef/EntityProp.h"
	#"${PROJECT_SOURCE_DIR}/src/logic/entitydef/EntityProp.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/entitydef/ScriptDefModule.h"
	"${PROJECT_SOURCE_DIR}/src/logic/entitydef/ScriptDefModule.cpp"
	#"${PROJECT_SOURCE_DIR}/src/logic/entitydef/Property.h"
	#"${PROJECT_SOURCE_DIR}/src/logic/entitydef/Property.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/entitydef/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${ENTITYDEF})

ADD_LIBRARY(EntityDef SHARED
	${ENTITYDEF}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(EntityDef slxml.lib shyloo.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET EntityDef PROPERTY FOLDER "logic")