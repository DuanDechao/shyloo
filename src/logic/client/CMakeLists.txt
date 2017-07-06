INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(CLIENT
	"${PROJECT_SOURCE_DIR}/src/logic/client/Client.h"
	"${PROJECT_SOURCE_DIR}/src/logic/client/Client.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/client/ClientSession.h"
	"${PROJECT_SOURCE_DIR}/src/logic/client/ClientSession.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/client/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${CLIENT})

ADD_LIBRARY(Client SHARED
	${CLIENT}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(Client slxml.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET Client PROPERTY FOLDER "core")