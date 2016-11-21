#ifndef _SL_SMARTPOINTER_H_
#define _SL_SMARTPOINTER_H_
namespace sl
{
template<class T>
inline void incrementReferenceCount(const T& obj)
{
	obj.incRef();
};

template<class T>
inline void decrementReferenceCount(const T& obj)
{
	obj.decRef();
};

template<class T>
class ConstSmartPointer
{
public:
	enum REF_TAG{STEAL_REF, NEW_REF};

	ConstSmartPointer(const T* obj = 0, REF_TAG tag = ConstSmartPointer::NEW_REF)
		:m_obj(obj)
	{
		if(obj)
		{
			if(tag == ConstSmartPointer::NEW_REF)
			{
				incrementReferenceCount(*m_obj);
			}
		}
	}

	ConstSmartPointer(const ConstSmartPointer<T>& P)
	{
		m_obj = P.get();
		if(m_obj)
			incrementReferenceCount(*m_obj);
	}

	~ConstSmartPointer()
	{
		if(m_obj)
			decrementReferenceCount(*m_obj);

		m_obj = 0;
	}

	void clear()
	{
		if(m_obj)
			decrementReferenceCount(*m_obj);

		m_obj = 0;
	}

	const T* get() const
	{
		return m_obj;
	}

	const T* operator()()
	{
		return m_obj;
	}

	ConstSmartPointer<T>& operator=(const ConstSmartPointer<T>& X)
	{
		if(m_obj != X.get())
		{
			const T* pOldObj = m_obj;
			m_obj = X.get();

			if(m_obj) incrementReferenceCount(*m_obj);
			if(pOldObj) decrementReferenceCount(*pOldObj);
		}

		return *this;
	}

	const T* operator->() const
	{
		return m_obj;
	}

	const T* operator*() const
	{
		return *m_obj;
	}

	friend bool operator==(const ConstSmartPointer<T>& A, const ConstSmartPointer<T>& B)
	{
		return A.m_obj == B.m_obj;
	}

	friend bool operator==(const ConstSmartPointer<T>& A, const T* B)
	{
		return A.m_obj == B;
	}

	friend bool operator==(const T* A, const ConstSmartPointer<T>& B)
	{
		return A == B.m_obj;
	}
	friend bool operator!=(const ConstSmartPointer<T>& A, const ConstSmartPointer<T>& B)
	{
		return A.m_obj != B.m_obj;
	}

	friend bool operator!=(const ConstSmartPointer<T>& A, const T* B)
	{
		return A.m_obj != B;
	}

	friend bool operator!=(const T* A, const ConstSmartPointer<T>& B)
	{
		return A = B.m_obj;
	}

	friend bool operator < (const ConstSmartPointer<T>& A, const ConstSmartPointer<T>& B)
	{
		return A.m_obj < B.m_obj;
	}

	friend bool operator <(const ConstSmartPointer<T>& A, const T* B)
	{
		return A.m_obj < B;
	}

	friend bool operator <(const T* A, const ConstSmartPointer<T>& B)
	{
		return A < B.m_obj;
	}

	friend bool operator >(const ConstSmartPointer<T>& A, const ConstSmartPointer<T>& B)
	{
		return A.m_obj > B.m_obj;
	}

	friend bool operator >(const ConstSmartPointer<T>& A, const T* B)
	{
		return A.m_obj > B;
	}

	friend bool operator>(const T* A, const ConstSmartPointer<T>& B)
	{
		return A > B.m_obj;
	}

	typedef const T * ConstSmartPointer<T>::*unspecified_bool_type;
	operator unspecified_bool_type() const
	{
		return m_obj == 0 ? 0 : &ConstSmartPointer<T>::m_obj;
	}

protected:
	const T*		m_obj;
};


template<class T>
class SmartPointer: public ConstSmartPointer<T>
{
public:
	typedef ConstSmartPointer<T> ConstProxy;

	SmartPointer(T* obj = 0, typename ConstProxy::REF_TAG tag = ConstProxy::NEW_REF)
		:ConstProxy(obj, tag)
	{}

	SmartPointer(const SmartPointer<T>& P): ConstProxy(P){}

	template<class DerivedType>
	SmartPointer(ConstSmartPointer<DerivedType>& dt)
		:ConstProxy(dt.get())
	{}

	SmartPointer<T>& operator=(const SmartPointer<T>& P)
	{
		ConstProxy::operator=(P);
		return *this;
	}

	template<class DerivedType>
	SmartPointer<T>& operator=(ConstSmartPointer<DerivedType>& dt)
	{
		ConstProxy::operator =(dt.get());
		return *this;
	}

	T* get() const
	{
		return const_cast<T*>(this->m_obj);
	}

	T* operator->() const
	{
		return const_cast<T*>(this->m_obj);
	}

	T& operator*() const
	{
		return *const_cast<T*>(this->m_obj);
	}

};
}
#endif