#ifndef SL_SLTYPE_H
#define SL_SLTYPE_H

//无符号整数
typedef unsigned char		uint8;          ///< 字节类型   8位无符号整数
typedef char				int8;			///< 字节类型   8位符号整数

typedef unsigned short		uint16;         ///< 字类型     16位无符号整数
typedef short				int16;          ///< 字类型     16位符号整数

typedef unsigned int        uint32;         ///< 双字类型   32位无符号整数
typedef int					int32;          ///< 双字类型   32位符号整数

typedef unsigned long long  uint64;         ///< 四字类型   64位无符号整数
//有符号整数
typedef signed long long    int64;          ///< 64位有符号整数

typedef wchar_t				wchar;          ///< 宽符号
typedef void*				pointer;        ///< 无类型指针
//using std::string;                          ///< 引用STL的String类型

typedef int64				datetime_t;     ///< 64位的时间类型
//typedef uint64				uid_t;          ///< 用户ID类型

#ifdef WIN32
#define SLAPI __stdcall

#ifdef SL_DLL_EXPORT
#define SL_DLL_API __declspec (dllexport)
#else
#define SL_DLL_API __declspec (dllimport)
#endif

#else
#define SLAPI
#define SL_DLL_API

#endif

#endif