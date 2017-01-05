/********************************************************************
	created:	2016/08/06
	created:	6:8:2016   16:14
	filename: 	c:\Users\ddc\Desktop\shyloo\libs\common\slmemorystream_converter.h
	file path:	c:\Users\ddc\Desktop\shyloo\libs\common
	file base:	slmemorystream_converter
	file ext:	h
	author:		ddc
	
	purpose:	×Ö½ÚÐò×ª»»
*********************************************************************/

#ifndef _SL_MEMORYSTREAM_CONVERTER_H_
#define _SL_MEMORYSTREAM_CONVERTER_H_
#include "slmulti_sys.h"

namespace sl
{
namespace MemoryStreamConverter
{
	template<size_t T>
	inline void convert(char* val)
	{
		std::swap(*val, *(val + T - 1));
		convert<T-2>(val + 1);
	}

	template<> inline void convert<0>(char*){}
	template<> inline void convert<1>(char*){}

	template<typename T>
	inline void apply(T* val)
	{
		convert<sizeof(T)>((char*)(val));
	}

	inline void convert(char* val, size_t size)
	{
		if(size < 2) return;

		std::swap(*val, *(val + size - 1));
		convert(val + 1, size - 2);
	}
}

#if SLENGINE_ENDIAN == SLENGINE_BIG_ENDIAN
template<typename T> inline void EndianConvert(T& val){MemoryStreamConverter::apply(&val);}
template<typename T> inline void EndianConvertReverse(T&) {}
#else
template<typename T> inline void EndianConvert(T&){}
template<typename T> inline void EndianConvertReverse(T& val) {MemoryStreamConverter::apply(&val);}
#endif


}// namespace sl

#endif