//����ѡ��궨��
#ifndef _SL_CONFIG_H_
#define _SL_CONFIG_H_

#if defined(__x86_64__) || defined(WIN64) || defined(AMD64)
	#define SL_CPU_64		///< 64λ���뻷��
#else
	#define SL_CPU_32		///< 32λ���뻷��
#endif

#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined(__MINGW__)
	#define SL_OS_WINDOWS   ///< windows����ϵͳ
#else
	#define SL_OS_LINUX		///< linux����ϵͳ
#endif

#ifdef SL_CPU_64
	#define SL_BIT64		///< SL64λ
#endif
#ifdef SL_CPU_32
	#define SL_BIT32	    ///< SL32λ
#endif

/** ���������ֽ��� */
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