INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(SCENE
	"${PROJECT_SOURCE_DIR}/src/logic/scene/SceneClientMgr.h"
	"${PROJECT_SOURCE_DIR}/src/logic/scene/SceneClientMgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/scene/SceneMgr.h"
	"${PROJECT_SOURCE_DIR}/src/logic/scene/SceneMgr.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/scene/Scene.h"
	"${PROJECT_SOURCE_DIR}/src/logic/scene/Scene.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/scene/AOI.h"
	"${PROJECT_SOURCE_DIR}/src/logic/scene/AOI.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/scene/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${SCENE})

ADD_LIBRARY(Scene SHARED
	${SCENE}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

TARGET_LINK_LIBRARIES(Scene
	${SL_LIB_DEBUG}
	${SL_LIB_RELEASE}
	${ATTR_LIBD}
	${ATTR_LIB}
)
IF(UNIX)
TARGET_LINK_LIBRARIES(Scene
  debug "${PROJECT_SOURCE_DIR}/build/linux/libAttrGetter.so"
  optimized "${PROJECT_SOURCE_DIR}/build/linux/libAttrGetter.so"
)
ENDIF(UNIX)

#SET_PROPERTY(TARGET Gate PROPERTY FOLDER "core")
SET_PROPERTY(TARGET Scene PROPERTY FOLDER "framework")
