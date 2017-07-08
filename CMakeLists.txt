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

IF(UNIX)
	SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/linux")
	LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/build/linux)
	SET(CMAKE_CXX_FLAGS "-std=c++0x -Wall -lpthread -rdynamic -ldl -Dlinux -lmysqlclient -Wl,-rpath,./")
	SET(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG -o0 -pg -L${PROJECT_SOURCE_DIR}/thirdlibrary/debug ")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2 -L${PROJECT_SOURCE_DIR}/thirdlibrary/release")
	SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -L${PROJECT_SOURCE_DIR}/thirdlibrary/release")
	MESSAGE(STATUS "Linux")
ENDIF(UNIX)


SET(SL_KERNEL_API
	"${PROJECT_SOURCE_DIR}/src/kernel/api/slikernel.h"
	"${PROJECT_SOURCE_DIR}/src/kernel/api/slimodule.h"
)

SET(SL_LIB_API
	"${PROJECT_SOURCE_DIR}/src/sllib/api/slnet.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/api/sltimer.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/api/slxml.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/api/sldb.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/api/slfile.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/api/slthread.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/api/slredis.h"
	"${PROJECT_SOURCE_DIR}/src/sllib/api/slshm.h"
)
SET(PUBLIC
	"${PROJECT_SOURCE_DIR}/src/public/slfile_utils.h"
	"${PROJECT_SOURCE_DIR}/src/public/sllog.h"
	#"${PROJECT_SOURCE_DIR}/src/public/slmemorystream.h"
	#"${PROJECT_SOURCE_DIR}/src/public/slmemorystream_converter.h"
	"${PROJECT_SOURCE_DIR}/src/public/slmutex.h"
	#"${PROJECT_SOURCE_DIR}/src/public/slobj_reload_check.h"
	#"${PROJECT_SOURCE_DIR}/src/public/slobjectpool.h"
	"${PROJECT_SOURCE_DIR}/src/public/slplatform.h"
	#"${PROJECT_SOURCE_DIR}/src/public/slrefcountable.h"
	"${PROJECT_SOURCE_DIR}/src/public/slsingleton.h"
	"${PROJECT_SOURCE_DIR}/src/public/slstring.h"
	#"${PROJECT_SOURCE_DIR}/src/public/slsmartpointer.h"
	"${PROJECT_SOURCE_DIR}/src/public/slstring_utils.h"
	"${PROJECT_SOURCE_DIR}/src/public/sltime.h"
	"${PROJECT_SOURCE_DIR}/src/public/sltype.h"
	"${PROJECT_SOURCE_DIR}/src/public/slpriority_queue.h"
	"${PROJECT_SOURCE_DIR}/src/public/slmulti_sys.h"
	"${PROJECT_SOURCE_DIR}/src/public/slmulti_sys.cpp"
	"${PROJECT_SOURCE_DIR}/src/public/slwin_sys.h"
	"${PROJECT_SOURCE_DIR}/src/public/sllinux_sys.h"
	"${PROJECT_SOURCE_DIR}/src/public/slxml_reader.h"
	"${PROJECT_SOURCE_DIR}/src/public/sltools.h"
	"${PROJECT_SOURCE_DIR}/src/public/sltools.cpp"
	"${PROJECT_SOURCE_DIR}/src/public/slargs.h"
	"${PROJECT_SOURCE_DIR}/src/public/slbinary_stream.h"
	"${PROJECT_SOURCE_DIR}/src/public/slcallback.h"
	"${PROJECT_SOURCE_DIR}/src/public/slcycle_queue.h"
	"${PROJECT_SOURCE_DIR}/src/public/slpool.h"
	"${PROJECT_SOURCE_DIR}/src/public/slbinary_map.h"
	"${PROJECT_SOURCE_DIR}/src/public/sllist.h"
	"${PROJECT_SOURCE_DIR}/src/public/slring_buffer.h"
	"${PROJECT_SOURCE_DIR}/src/public/slshm_create.h"
)

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/sllib)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/kernel)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/attrgen)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic)
