//定义简单的单件类
#ifndef _SL_SINGLETON_H_
#define _SL_SINGLETON_H_
#if _MSC_VER > 1000
#pragma once
#endif
#include <assert.h>
namespace sl
{
template<typename T>
class CSingleton
{
public:
	CSingleton(void){}
	~CSingleton(void) {}

	static T& getSingleton(void) {assert(m_singleton); return (*m_singleton);}
	static T* getSingletonPtr(void) {return m_singleton;}
};

#define SL_SINGLETON_INIT(TYPE)							\
	template <> TYPE * CSingleton<TYPE>::m_singleton = 0;	\

}// namespace sl

#endif