INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/thirdlibrary/python/Include"
)

SET(OBJECTDEF
    "${PROJECT_SOURCE_DIR}/src/logic/objectdef/PyDataType.h"
    "${PROJECT_SOURCE_DIR}/src/logic/objectdef/PyDataType.cpp"
    "${PROJECT_SOURCE_DIR}/src/logic/objectdef/DataTypeMgr.h"
    "${PROJECT_SOURCE_DIR}/src/logic/objectdef/DataTypeMgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/objectdef/ObjectDef.h"
	"${PROJECT_SOURCE_DIR}/src/logic/objectdef/ObjectDef.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/objectdef/ObjectDefModule.h"
	"${PROJECT_SOURCE_DIR}/src/logic/objectdef/ObjectDefModule.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/objectdef/FixedDict.h"
	"${PROJECT_SOURCE_DIR}/src/logic/objectdef/FixedDict.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/objectdef/FixedArray.h"
	"${PROJECT_SOURCE_DIR}/src/logic/objectdef/FixedArray.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/objectdef/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(pyscript FILES ${PYSCRIPT})
SOURCE_GROUP(\\ FILES ${OBJECTDEF})

ADD_LIBRARY(ObjectDef SHARED
    ${OBJECTDEF}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
	${PYSCRIPT}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(ObjectDef slxml.lib shyloo.lib)
ENDIF(WIN32)

IF(UNIX)
TARGET_LINK_LIBRARIES(ObjectDef libpython3.so)
ENDIF(UNIX)

SET_PROPERTY(TARGET ObjectDef PROPERTY FOLDER "logic")
