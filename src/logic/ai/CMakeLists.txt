INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/composites"
)
SET(COMPOSITES
	"${PROJECT_SOURCE_DIR}/src/logic/ai/composites/Selector.h"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/composites/Selector.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/composites/Sequence.h"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/composites/Sequence.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/composites/Parallel.h"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/composites/Parallel.cpp"
)

SET(ACTIONS
	"${PROJECT_SOURCE_DIR}/src/logic/ai/actions/Action.h"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/actions/Action.cpp"
)

SET(AI
	"${PROJECT_SOURCE_DIR}/src/logic/ai/AI.h"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/AI.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/AINode.h"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/AINode.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/AILoader.h"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/AILoader.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/AICondition.h"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/AICondition.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/AIMachine.h"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/AIMachine.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/ai/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(composites FILES ${COMPOSITES})
SOURCE_GROUP(actions FILES ${ACTIONS})
SOURCE_GROUP(\\ FILES ${AI})

ADD_LIBRARY(AI SHARED
	${AI}
	${COMPOSITES}
	${ACTIONS}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(AI AttrGetter.lib slxml.lib)
ENDIF(WIN32)

#IF(UNIX)
#TARGET_LINK_LIBRARIES(AI
# debug "${PROJECT_SOURCE_DIR}/build/linux/libAttrGetter.so"
# optimized "${PROJECT_SOURCE_DIR}/build/linux/libAttrGetter.so"
#)
#ENDIF(UNIX)

SET_PROPERTY(TARGET AI PROPERTY FOLDER "framework")
