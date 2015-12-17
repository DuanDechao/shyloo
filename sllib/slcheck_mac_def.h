/**
* 定义一些对ret 检查处理的宏
*/

#ifndef _SL_CHECK_MAC_DEF_H_
#define _SL_CHECK_MAC_DEF_H_

#include "slexception.h"

#define CHECK_RETURN(iRet)  do { if (iRet) { return iRet; } } while(0)
#define CHECK_EXIT(iRet) do { if (iRet) { exit(iRet); } } while(0)
#define CHECK_THROW(iRet) do { if (iRet) { SL_THROW(iRet); } } while(0)
#define CHECK_THROWEX(iRet,...) do { if (iRet) { printf(__VA_ARGS__); SL_THROW(iRet); } } while(0)

#endif  //_LZ_CHECK_MAC_DEF_H_