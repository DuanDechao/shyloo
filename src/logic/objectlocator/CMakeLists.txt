INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(OBJECTLOCATOR
	"${PROJECT_SOURCE_DIR}/src/logic/objectlocator/ObjectLocator.h"
	"${PROJECT_SOURCE_DIR}/src/logic/objectlocator/ObjectLocator.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/objectlocator/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${OBJECTLOCATOR})

ADD_LIBRARY(ObjectLocator SHARED
	${OBJECTLOCATOR}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

TARGET_LINK_LIBRARIES(ObjectLocator
	${SL_LIB_DEBUG}
	${SL_LIB_RELEASE}
	${ATTR_LIBD}
	${ATTR_LIB}
)

TARGET_LINK_LIBRARIES(ObjectLocator "AttrGetter.lib")
SET_PROPERTY(TARGET ObjectLocator PROPERTY FOLDER "framework")