INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/thirdlibrary/python/Include"
)

SET(PYSCRIPT
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/py_macros.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/scriptobject.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/scriptobject.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/scriptstderr.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/scriptstderr.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/scriptstdout.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/scriptstdout.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/scriptstdouterr.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/scriptstdouterr.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/scriptstdouterrhook.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/scriptstdouterrhook.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/pickler.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/pickler.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/py_gc.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/pyscript/py_gc.cpp"
)

SET(PYTHONENGINE
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/PythonEngine.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/PythonEngine.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/PyScript.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/PyScript.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonengine/main.cpp"
)


SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(pyscript FILES ${PYSCRIPT})
SOURCE_GROUP(\\ FILES ${PYTHONENGINE})

ADD_LIBRARY(PythonEngine SHARED
	${PYTHONENGINE}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
	${PYSCRIPT}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(PythonEngine slxml.lib shyloo.lib slpyscript.lib)
ENDIF(WIN32)

IF(UNIX)
TARGET_LINK_LIBRARIES(PythonEngine libpython3.4m.so)
ENDIF(UNIX)

SET_PROPERTY(TARGET PythonEngine PROPERTY FOLDER "logic")
