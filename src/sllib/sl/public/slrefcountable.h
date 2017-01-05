#ifndef _SL_REFCOUNTABLE_H_
#define _SL_REFCOUNTABLE_H_
#include "slmulti_sys.h"

namespace sl
{
class RefCountable
{
public:
	inline void incRef(void) const
	{
		++m_refCount;
	}

	inline void decRef(void) const
	{
		int currRef = --m_refCount;
		if( 0>= currRef)
			onRefOver();
		 
	}

	virtual void onRefOver(void) const
	{
		delete const_cast<RefCountable*>(this);
	}

	void setRefCount(int n)
	{
		m_refCount = n;
	}

	int getRefCount(void ) const
	{
		return m_refCount;
	}
protected:
	RefCountable(void): m_refCount(0){}

	virtual ~RefCountable(void){}
protected:
	volatile mutable long m_refCount;
};

#ifdef SL_OS_WINDOWS
class SafeRefCountable
{
public:
	inline void incRef(void) const
	{
		::InterlockedIncrement(&m_refCount);
	}

	inline void decRef(void) const
	{
		long currRef = ::InterlockedDecrement(&m_refCount);
		if(0 >= currRef)
			onRefOver();
	}

	virtual void onRefOver(void) const
	{
		delete const_cast<SafeRefCountable*>(this);
	}

	void setRefCount(long n)
	{
		InterlockedExchange((long*)&m_refCount, n);
	}

	int getRefCount(void) const
	{
		return InterlockedExchange((long*)&m_refCount, m_refCount);
	}
protected:
	SafeRefCountable(void):m_refCount(0){}

	virtual ~SafeRefCountable(void){}
protected:
	volatile mutable long m_refCount;
};
#endif // SL_OS_WINDOWS

}
#endif