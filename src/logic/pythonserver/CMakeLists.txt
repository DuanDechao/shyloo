INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/public"
	"${PROJECT_SOURCE_DIR}/src/kernel/api"
	"${PROJECT_SOURCE_DIR}/src/logic/interface"
	"${PROJECT_SOURCE_DIR}/src/logic/define"
	"${PROJECT_SOURCE_DIR}/src/sllib/api"
	"${PROJECT_SOURCE_DIR}/thirdlibrary/python/Include"
)

SET(PYSCRIPT
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/py_macros.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/scriptobject.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/scriptobject.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/vector2.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/vector2.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/vector3.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/vector3.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/vector4.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/vector4.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/pickler.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/pickler.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/py_memorystream.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/py_memorystream.inl"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/py_memorystream.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/map.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/map.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/sequence.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/sequence.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/py_gc.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/py_gc.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/pyscript.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/pyscript/pyscript.cpp"
)

SET(PYTHONSERVER
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/EntityScriptObject.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/EntityScriptObject.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/ScriptDefModule.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/ScriptDefModule.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/EntityMailBox.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/EntityMailBox.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/RemoteEntityMethod.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/RemoteEntityMethod.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/PythonServer.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/PythonServer.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/Base.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/Base.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/Entity.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/Entity.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/Proxy.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/Proxy.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/EntityGarbages.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/EntityGarbages.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/Entities.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/Entities.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/ScriptTimers.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/ScriptTimers.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/GlobalData.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/GlobalData.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/PyDataType.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/PyDataType.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/FixedDict.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/FixedDict.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/FixedArray.h"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/FixedArray.cpp"
	 "${PROJECT_SOURCE_DIR}/src/logic/pythonserver/main.cpp"
)


SOURCE_GROUP(api FILES ${SL_KERNEL_API})
SOURCE_GROUP(public FILES ${PUBLIC})
SOURCE_GROUP(interface FILES ${SL_LOGIC_INTERFACE})
SOURCE_GROUP(define FILES ${SL_LOGIC_DEFINE})
SOURCE_GROUP(pyscript FILES ${PYSCRIPT})
SOURCE_GROUP(\\ FILES ${PYTHONSERVER})

ADD_LIBRARY(PythonServer SHARED
	${PYTHONSERVER}
	${SL_KERNEL_API}
	${PUBLIC}
	${SL_LOGIC_INTERFACE}
	${SL_LOGIC_DEFINE}
	${PYSCRIPT}
)

IF(WIN32)
TARGET_LINK_LIBRARIES(PythonServer slxml.lib shyloo.lib slpyscript.lib)
ENDIF(WIN32)

IF(UNIX)
TARGET_LINK_LIBRARIES(PythonServer libpython3.so)
ENDIF(UNIX)
SET_PROPERTY(TARGET PythonServer PROPERTY FOLDER "logic")
