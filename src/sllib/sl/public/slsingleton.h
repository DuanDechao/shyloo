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
protected:
	static T* m_singleton;

public:
	CSingleton(void)
	{
		assert(!m_singleton);
#if defined(_MSC_VER) && _MSC_VER < 1200
		int offset = (int)(T*)1 - (int)(CSingleton<T>*)(T*)1;
		m_singleton = (T*)((int)this + offset);
#else
		m_singleton = static_cast<T*>(this);
#endif
	}

	~CSingleton(void) {assert(m_singleton); m_singleton = 0;}

	static T& getSingleton(void) {assert(m_singleton); return (*m_singleton);}
	static T* getSingletonPtr(void) {return m_singleton;}
};

#define SL_SINGLETON_INIT(TYPE)							\
	template <> TYPE * CSingleton<TYPE>::m_singleton = 0;	\

}// namespace sl

#endif