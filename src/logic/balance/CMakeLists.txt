INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(BALANCE
	"${PROJECT_SOURCE_DIR}/src/logic/balance/Balance.h"
	"${PROJECT_SOURCE_DIR}/src/logic/balance/Balance.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/balance/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${BALANCE})

ADD_LIBRARY(Balance SHARED
	${BALANCE}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(Balance slxml.lib)
ENDIF(WIN32)

SET_PROPERTY(TARGET Balance PROPERTY FOLDER "framework")