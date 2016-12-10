#ifndef SL_PRIORITY_QUEUE_H
#define SL_PRIORITY_QUEUE_H

namespace sl
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

	bool empty() const {return m_dataContainer.empty();}
	size_type size() const {return m_dataContainer.size();}

	const value_type& top() const {return m_dataContainer.front();}

	void push(const value_type& x)
	{
		m_dataContainer.push_back(x);
		std::push_heap(m_dataContainer.begin(), m_dataContainer.end(), Comparator());
	}

	void pop()
	{
		std::pop_heap(m_dataContainer.begin(), m_dataContainer.end(),Comparator());
		m_dataContainer.pop_back();
	}

	T unsafePopBack()
	{
		T pTimer = m_dataContainer.back();
		m_dataContainer.pop_back();
		return pTimer;
	}

	Container& getContainer() {return m_dataContainer;}

	void makeHeap()
	{
		std::make_heap(m_dataContainer.begin(), m_dataContainer.end(),Comparator());
	}

	void clear()
	{
		m_dataContainer.clear();
	}

	T operator[] (const size_t i) const {return m_dataContainer[i];}

private:
private:
	Container		m_dataContainer;
};

}
#endif