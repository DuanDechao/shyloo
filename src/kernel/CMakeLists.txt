
IF(WIN32)
INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/kernel/interface"
	"${PROJECT_SOURCE_DIR}/src/kernel"
)
ENDIF(WIN32)

SET(SL_KERNEL_INTERFACE
	"${PROJECT_SOURCE_DIR}/src/kernel/interface/slicore.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/interface/slinet_engine.h"
)

SET(SL_KERNEL_NET
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_engine.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_engine.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_session.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_session.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_session_factory.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/net/slnet_session_factory.cpp"
)

SET(SL_KERNEL
	"${PROJECT_SOURCE_DIR}/src/kernel/slkernel.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/slkernel.cpp"
	"${PROJECT_SOURCE_DIR}/src/kernel/slmain.cpp"
)
SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(src\\interface FILES ${SL_KERNEL_INTERFACE})
SOURCE_GROUP(src\\net FILES ${SL_KERNEL_NET})
SOURCE_GROUP(\\ FILES ${SL_KERNEL})

IF(WIN32)
ADD_EXECUTABLE(shyloo
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_KERNEL_INTERFACE}
	${SL_KERNEL_NET}
	${SL_KERNEL}
)
ENDIF(WIN32)

IF(WIN32)
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/windows")
ENDIF(WIN32)
