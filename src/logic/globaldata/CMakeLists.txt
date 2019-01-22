INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(GLOBALDATA
	"${PROJECT_SOURCE_DIR}/src/logic/globaldata/GlobalData.h"
	"${PROJECT_SOURCE_DIR}/src/logic/globaldata/GlobalData.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/globaldata/GlobalDataServer.h"
	"${PROJECT_SOURCE_DIR}/src/logic/globaldata/GlobalDataServer.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/globaldata/GlobalDataClient.h"
	"${PROJECT_SOURCE_DIR}/src/logic/globaldata/GlobalDataClient.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/globaldata/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${GLOBALDATA})

ADD_LIBRARY(GlobalData SHARED
	${GLOBALDATA}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(GlobalData AttrGetter.lib)
ENDIF(WIN32)

IF(UNIX)
TARGET_LINK_LIBRARIES(GlobalData)
ENDIF(UNIX)


SET_PROPERTY(TARGET GlobalData PROPERTY FOLDER "framework")
