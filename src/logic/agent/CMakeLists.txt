INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(AGENT
	"${PROJECT_SOURCE_DIR}/src/logic/agent/Agent.h"
	"${PROJECT_SOURCE_DIR}/src/logic/agent/Agent.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/agent/AgentSession.h"
	"${PROJECT_SOURCE_DIR}/src/logic/agent/AgentSession.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/agent/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${AGENT})

ADD_LIBRARY(Agent SHARED
	${AGENT}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(Agent slxml.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET Agent PROPERTY FOLDER "core")