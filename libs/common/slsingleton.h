//定义简单的单件类
#ifndef _SL_SINGLETON_H_
#define _SL_SINGLETON_H_
namespace sl
{
	//简单的指针模板类
	//不负责指针创建，但会自动回收指针
	template<typename T>
	class CAutoPtr
	{
	public:
		CAutoPtr(): m_auto_ptr(0){}
		CAutoPtr(T* ptr): m_auto_ptr(ptr){}
		CAutoPtr(const CAutoPtr& obj): m_auto_ptr(obj.get()) {}
		~CAutoPtr(){reset(0);}

		//赋值函数
		CAutoPtr& operator= (const CAutoPtr& obj)
		{
			m_auto_ptr = obj.get();
			return *this;
		}
		T* get() const {return m_auto_ptr;}
		T* operator() () {return get();}

		void reset(T* ptr)
		{
			if(m_auto_ptr == ptr)
			{
				return;
			}
			if(m_auto_ptr)
			{
				delete m_auto_ptr;
			}
			m_auto_ptr = ptr;
		}
	private:
		T* m_auto_ptr;

	}; // class AutoPtr
	//单件类
	/*
		X可以让单件类创建多个
	*/
	template<typename T, int X = 0>
	class CSingleton
	{
	private:
		CSingleton() {}
		CSingleton(const CSingleton& ){}
		~CSingleton(){}
		CSingleton& operator= (const CSingleton&) {return *this;}
	public:
		static T* Instance()
		{
			static CAutoPtr<T> autoptr;
			if(autoptr.get() == 0)
			{
				autoptr.reset(new T);
			}
			return autoptr.get();
		}
	};

}// namespace sl

#endif