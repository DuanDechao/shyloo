#ifndef SL_PRIORITY_QUEUE_H
#define SL_PRIORITY_QUEUE_H
#include "sltimer_base.h"
namespace sl
{
namespace timer
{
template<class T>
class DefaultComparator
{
public:
	bool operator()(const T a, const T b)
	{
		return a > b;
	}
};
template<class T, class Comparator = DefaultComparator>
class PriorityQueue
{
public:
	typedef std::vector<T> Container;

	typedef typename Container::value_type value_type;
	typedef typename Container::size_type  size_type;

	bool empty() const {return m_TimeContainer.empty();}
	size_type size() const {return m_TimeContainer.size();}

	const value_type& top() const {return m_TimeContainer.front();}

	void push(const value_type& x)
	{
		m_TimeContainer.push_back(x);
		std::push_heap(m_TimeContainer.begin(), m_TimeContainer.end(), Comparator());
	}

	void pop()
	{
		std::pop_heap(m_TimeContainer.begin(), m_TimeContainer.end(),Comparator());
		m_TimeContainer.pop_back();
	}

	T unsafePopBack()
	{
		T pTimer = m_TimeContainer.back();
		m_TimeContainer.pop_back();
		return pTimer;
	}

	Container& getContainer() {return m_TimeContainer;}

	void makeHeap()
	{
		std::make_heap(m_TimeContainer.begin(), m_TimeContainer.end(),Comparator());
	}

	void clear()
	{
		m_TimeContainer.clear();
	}

	T operator[] (const size_t i) const {return m_TimeContainer[i];}

private:
private:
	Container		m_TimeContainer;
};
}
}
#endif