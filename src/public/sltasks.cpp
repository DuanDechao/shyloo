#include "sltasks.h"
namespace sl
{
	Tasks::Tasks():m_container(){}
	Tasks::~Tasks(){}

	void Tasks::add(Task* pTask)
	{
		m_container.push_back(pTask);
	}

	bool Tasks::cancel(Task* pTask)
	{
		Container::iterator iter = std::find(m_container.begin(), m_container.end(), pTask);
		if(iter != m_container.end())
		{
			m_container.erase(iter);
			return true;
		}
		return false;
	}

	void Tasks::process()
	{
		Container::iterator iter = m_container.begin();
		while(iter != m_container.end())
		{
			Task* pTask = *iter;
			if(!pTask->process())
				iter = m_container.erase(iter);
			else
				++iter;
		}
	}
}