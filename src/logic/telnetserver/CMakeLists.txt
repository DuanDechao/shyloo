INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(TELNETSERVER
	 "${PROJECT_SOURCE_DIR}/src/logic/telnetserver/TelnetServer.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/telnetserver/TelnetServer.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/telnetserver/TelnetSession.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/telnetserver/TelnetSession.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/telnetserver/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${TELNETSERVER})

ADD_LIBRARY(TelnetServer SHARED
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
	${TELNETSERVER}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(TelnetServer slxml.lib shyloo.lib)
ENDIF(WIN32)

IF(UNIX)
TARGET_LINK_LIBRARIES(TelnetServer)
ENDIF(UNIX)
SET_PROPERTY(TARGET TelnetServer PROPERTY FOLDER "logic")
