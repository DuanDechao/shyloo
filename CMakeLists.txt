#cmake file 
cmake_minimum_required(VERSION 2.4)
PROJECT(shyloo)
option(USE_32BITS OFF)

IF(WIN32)
	LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/build/windows)
	SET(CMAKE_C_FLAGS_DEBUG_INIT "/D_DEBUG /Zm8192000 /MTd /Zi /Ob0 /Od /RTC1")
	SET(CMAKE_C_FLAGS_MINSIZEREL_INIT "/MT /Zm8192000 /O1 /Ob1 /D NDEBUG")
	SET(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "/MT /Zm8192000 /Zi /O2 /Ob1 /D NDEBUG")
	SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/windows")
	MESSAGE(STATUS "WINDOWS")
	IF(CMAKE_CL_64)
		MESSAGE(STATUS "x64")
	ELSE()
		MESSAGE(STATUS "x86")
	ENDIF(CMAKE_CL_64)
	SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
ENDIF(WIN32)

SET(LIBS
	#"${PROJECT_SOURCE_DIR}/src/libs/common/slassert.h"
	)
SET(COMMON
	"${PROJECT_SOURCE_DIR}/src/common/slassert.h"
	"${PROJECT_SOURCE_DIR}/src/common/slbase.h"
	"${PROJECT_SOURCE_DIR}/src/common/slbase_define.h"
	"${PROJECT_SOURCE_DIR}/src/common/slcode_convert.h"
	"${PROJECT_SOURCE_DIR}/src/common/slcommon.h"
	"${PROJECT_SOURCE_DIR}/src/common/slcommon.cpp"
	"${PROJECT_SOURCE_DIR}/src/common/slconfig.h"
	"${PROJECT_SOURCE_DIR}/src/common/slfile.h"
	"${PROJECT_SOURCE_DIR}/src/common/slfile_utils.h"
	"${PROJECT_SOURCE_DIR}/src/common/sllog.h"
	"${PROJECT_SOURCE_DIR}/src/common/slmemorystream.h"
	"${PROJECT_SOURCE_DIR}/src/common/slmemorystream.cpp"
	"${PROJECT_SOURCE_DIR}/src/common/slmemorystream_converter.h"
	"${PROJECT_SOURCE_DIR}/src/common/slmulti_object_mgr.h"
	"${PROJECT_SOURCE_DIR}/src/common/slmutex.h"
	"${PROJECT_SOURCE_DIR}/src/common/slobj_reload_check.h"
	"${PROJECT_SOURCE_DIR}/src/common/slobject_mgr.h"
	"${PROJECT_SOURCE_DIR}/src/common/slobjectpool.h"
	"${PROJECT_SOURCE_DIR}/src/common/slplatform.h"
	"${PROJECT_SOURCE_DIR}/src/common/slrefcountable.h"
	"${PROJECT_SOURCE_DIR}/src/common/slsingleton.h"
	"${PROJECT_SOURCE_DIR}/src/common/slsize_string.h"
	"${PROJECT_SOURCE_DIR}/src/common/slsmartpointer.h"
	"${PROJECT_SOURCE_DIR}/src/common/slstring_utils.h"
	"${PROJECT_SOURCE_DIR}/src/common/sltask.h"
	"${PROJECT_SOURCE_DIR}/src/common/sltasks.h"
	"${PROJECT_SOURCE_DIR}/src/common/sltasks.cpp"
	"${PROJECT_SOURCE_DIR}/src/common/sltime.h"
	"${PROJECT_SOURCE_DIR}/src/common/sltimer.h"
	"${PROJECT_SOURCE_DIR}/src/common/sltimer.cpp"
	"${PROJECT_SOURCE_DIR}/src/common/sltimestamp.h"
	"${PROJECT_SOURCE_DIR}/src/common/sltimestamp.cpp"
	"${PROJECT_SOURCE_DIR}/src/common/sltype.h"	
)

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/kernel)