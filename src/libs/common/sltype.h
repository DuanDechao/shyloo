//定义shyloo的基本数据类型
#ifndef _SL_TYPE_H_
#define _SL_TYPE_H_

#include <string>
#include <cstdio>
#include <cstdlib>

namespace sl
{
	//无符号整数
	typedef unsigned char		byte;           ///< 字节类型   8位无符号整数

	typedef unsigned short		ushort;         ///< 字类型     16位无符号整数
	typedef unsigned int        uint;           ///< 双字类型   32位无符号整数
	typedef unsigned long long  uint64;         ///< 四字类型   64位无符号整数

	//有符号整数
	typedef signed long long    int64;          ///< 64位有符号整数
	
	typedef wchar_t				wchar;          ///< 宽符号
	typedef void*				pointer;        ///< 无类型指针
	using std::string;                          ///< 引用STL的String类型

	typedef int64				datetime_t;     ///< 64位的时间类型
	typedef uint64				uid_t;          ///< 用户ID类型


}// namespace sl
#endif