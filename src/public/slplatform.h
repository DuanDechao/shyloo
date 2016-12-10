//编译选项宏定义
#ifndef _SL_CONFIG_H_
#define _SL_CONFIG_H_

#if defined(__x86_64__) || defined(WIN64) || defined(AMD64)
	#define SL_CPU_64		///< 64位编译环境
#else
	#define SL_CPU_32		///< 32位编译环境
#endif

#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined(__MINGW__)
	#define SL_OS_WINDOWS   ///< windows操作系统
#else
	#define SL_OS_LINUX		///< linux操作系统
#endif

#ifdef SL_CPU_64
	#define SL_BIT64		///< SL64位
#endif
#ifdef SL_CPU_32
	#define SL_BIT32	    ///< SL32位
#endif

/** 定义引擎字节序 */
#define SLENGINE_LITTLE_ENDIAN							0
#define SLENGINE_BIG_ENDIAN								1
#if !defined(SLENGINE_ENDIAN)
#  if defined (USE_BIG_ENDIAN)
#    define SLENGINE_ENDIAN SLENGINE_BIG_ENDIAN
#  else 
#    define SLENGINE_ENDIAN SLENGINE_LITTLE_ENDIAN
#  endif 
#endif

#endif