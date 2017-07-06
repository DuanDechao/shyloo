//定义简单的单件类
#ifndef _SL_SINGLETON_H_
#define _SL_SINGLETON_H_
#if _MSC_VER > 1000
#pragma once
#endif
#include <assert.h>
#include "slmulti_sys.h"
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


template <typename T>
class SLSingleton{
public:
	SLSingleton() {}
	virtual ~SLSingleton() {}

	inline static T* getInstance(){
		static T* instance = nullptr;

		if (instance == nullptr){
			instance = NEW T;
			if (!instance->ready()){
				delete instance;
				instance = nullptr;
			}
		}

		return instance;
	}
};

template<typename T>
class SLHolder{
public:
	SLHolder() { s_instance = (T*)this; }
	virtual ~SLHolder() {}

	inline static T* getInstance(){ return s_instance; }
private:
	static T* s_instance;
};

template<typename T>
T* SLHolder<T>::s_instance = nullptr;

}// namespace sl

#endif