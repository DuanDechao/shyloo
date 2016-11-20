//定义简单的单件类
#ifndef _SL_SINGLETON_H_
#define _SL_SINGLETON_H_
#if _MSC_VER > 1000
#pragma once
#endif
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


	///简单的指针模板类
    /**
       不负责指针创建，但会自动回收指针，应仔细使用
    */
//    template<typename T>
//    class CAutoPtr
//    {
//    public:
//        CAutoPtr() : m_auto_ptr(0) {}
//        CAutoPtr(T* ptr) : m_auto_ptr(ptr) {}
//        CAutoPtr(const CAutoPtr& obj) : m_auto_ptr(obj.get()) {}
//        ~CAutoPtr() { reset (0); }
//
//        //拷贝函数
//        CAutoPtr& operator= (const CAutoPtr& obj)
//        {
//            this->m_auto_ptr = obj.get();
//            return *this;
//        }
//
//        T* get() const { return m_auto_ptr; }
//        T* operator() () { return get(); }
//
//        void reset(T* ptr)
//        {
//            if (m_auto_ptr == ptr)
//            {
//                return;
//            }
//
//            if (m_auto_ptr)
//            {
//                delete m_auto_ptr;
//            }
//
//            m_auto_ptr = ptr;
//        }
//
//    private:
//        T* m_auto_ptr;
//    };
//
/////单件类
//    /**
//        X可以让单件类创建多个
//    */
//    template<typename T, int X = 0>
//	class CSingleton 
//    {
//    private:
//        CSingleton() {}
//        CSingleton(const CSingleton&) {}
//        ~CSingleton() {}
//        CSingleton& operator= (const CSingleton &) { return *this; } 
//
//    public:
//        static T* Instance()
//        {
//            static CAutoPtr<T> autoptr;
//            if(autoptr.get() == 0)
//            {
//                autoptr.reset(new T);
//            }
//            return autoptr.get();
//        }
//    };

}// namespace sl

#endif