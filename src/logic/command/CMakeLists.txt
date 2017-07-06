INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
)

SET(COMMAND
	"${PROJECT_SOURCE_DIR}/src/logic/command/OCCommand.h"
	"${PROJECT_SOURCE_DIR}/src/logic/command/OCCommand.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/command/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${COMMAND})

ADD_LIBRARY(Command SHARED
	${COMMAND}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

TARGET_LINK_LIBRARIES(Command
	${SL_LIB_DEBUG}
	${SL_LIB_RELEASE}
	${ATTR_LIBD}
	${ATTR_LIB}
)

IF(UNIX)
TARGET_LINK_LIBRARIES(Command
  debug "${PROJECT_SOURCE_DIR}/build/linux/libAttrGetter.so"
  optimized "${PROJECT_SOURCE_DIR}/build/linux/libAttrGetter.so"
)
ENDIF(UNIX)


SET_PROPERTY(TARGET Command PROPERTY FOLDER "framework")
