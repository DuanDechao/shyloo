INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(PACKETSENDER
	"${PROJECT_SOURCE_DIR}/src/logic/packetsender/PacketSender.h"
	"${PROJECT_SOURCE_DIR}/src/logic/packetsender/PacketSender.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/packetsender/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${PACKETSENDER})

ADD_LIBRARY(PacketSender SHARED
	${PACKETSENDER}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

SET_PROPERTY(TARGET PacketSender PROPERTY FOLDER "framework")