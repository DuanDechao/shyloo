//����򵥵ĵ�����
#ifndef _SL_SINGLETON_H_
#define _SL_SINGLETON_H_
namespace sl
{
	//�򵥵�ָ��ģ����
	//������ָ�봴���������Զ�����ָ��
	template<typename T>
	class CAutoPtr
	{
	public:
		CAutoPtr(): m_auto_ptr(0){}
		CAutoPtr(T* ptr): m_auto_ptr(ptr){}
		CAutoPtr(const CAutoPtr& obj): m_auto_ptr(obj.get()) {}
		~CAutoPtr(){reset(0);}

		//��ֵ����
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
	//������
	/*
		X�����õ����ഴ�����
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