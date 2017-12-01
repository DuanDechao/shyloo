INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/src/sllib/slpyscript"
	"${PROJECT_SOURCE_DIR}/src/sllib/python/Include"
	"${PROJECT_SOURCE_DIR}/src/sllib/python/PC"
)

SET(PYTHON_ENGINE
	"${PROJECT_SOURCE_DIR}/src/logic/pythonengine/PythonEngine.h"
	"${PROJECT_SOURCE_DIR}/src/logic/pythonengine/PythonEngine.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/pythonengine/main.cpp"
)

SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(\\ FILES ${PYTHON_ENGINE})

ADD_LIBRARY(PythonEngine SHARED
	${PYTHON_ENGINE}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(PythonEngine slxml.lib slpyscript.lib)
ENDIF(WIN32)

#SET_PROPERTY(TARGET Gate PROPERTY FOLDER "core")
#SET_PROPERTY(TARGET Gate PROPERTY FOLDER "framework")
SET_PROPERTY(TARGET PythonEngine PROPERTY FOLDER "core")