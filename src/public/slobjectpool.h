/********************************************************************
	created:	2016/08/03
	created:	3:8:2016   13:42
	filename: 	d:\Projects\shyloo\libs\common\slobjectpool.h
	file path:	d:\Projects\shyloo\libs\common
	file base:	slobjectpool
	file ext:	h
	author:		ddc
	
	purpose:	
******************************************************************/
#ifndef _SL_OBJECT_POOL_H_
#define _SL_OBJECT_POOL_H_
#include <set>
#include <string>
#include <queue>
#include <list>
#include <map>
#include "sltype.h"
#include "sltime.h"
namespace sl
{
#define OBJECT_POOL_INIT_SIZE			16
#define OBJECT_POOL_INIT_MAX_SIZE		OBJECT_POOL_INIT_SIZE * 1024

//ÿ5���Ӽ��һ������
#define OBJECT_POOL_REDUCING_TIME_OUT	5 * 60 * 1000

template<typename T>
class SmartPoolObject;

/*
	һЩ�����Ƶ���Ĵ������磺MemoeryStream��Bundle��TCPPacket�ȵ�
	�������ض�ͨ����������ֵ��Ч��Ԥ����ǰ��������һЩ���󻺴��������õ�ʱֱ��
	�ڶ�����л�ȡһ��δ��ʹ�õĶ��󼴿�
*/
template<typename T>
class CObjectPool
{
public:
	typedef	std::list<T*> OBJECTS;

	CObjectPool(std::string name)
		:m_objects(),
		 m_max(OBJECT_POOL_INIT_MAX_SIZE),
		 m_isDestroyed(false),
		 m_name(name),
		 m_total_allocs(0),
		 m_obj_count(0),
		 m_lastReducingCheckTime(getTimeMilliSecond())
	{}

	CObjectPool(std::string name, unsigned int preAssignVal, size_t max)
		:m_objects(),
		 m_max(max == 0 ? 1 : max),
		 m_isDestroyed(false),
		 m_name(name),
		 m_total_allocs(0),
		 m_obj_count(0),
		 m_lastReducingCheckTime(getTimeMilliSecond())
	{}

	~CObjectPool()
	{
		
	}

	void Destroy()
	{
		m_isDestroyed = true;
		typename OBJECTS::iterator iter = m_objects.begin();
		for (; iter != m_objects.end(); ++iter)
		{
			if(!(*iter)->destructorPoolObject()){
				delete (*iter);
			}
		}
		m_objects.clear();
		m_obj_count = 0;
	}

	const OBJECTS& objects(void) const
	{
		return m_objects;
	}

	void assignObjs(uint32 preAssignVal = OBJECT_POOL_INIT_SIZE)
	{
		for (uint32 i = 0; i < preAssignVal; ++i)
		{
			m_objects.push_back(new T);
			++m_total_allocs;
			++m_obj_count;
		}
	}

	/*
		ǿ�ƴ���һ��ָ�����͵Ķ�������������Ѿ������򷵻����еģ�����
		����һ���µģ������������Ǽ̳���T��
	*/

	template<typename T1>
	T* FetchObj(void)
	{
		while(true)
		{
			if(m_obj_count > 0)
			{
				T* t = static_cast<T1*>(*m_objects.begin());
				m_objects.pop_front();
				--m_obj_count;
				t->onEnabledPoolObject();
				return t;
			}
			assignObjs();
		}

		return NULL;
	}

	/*
	  ����һ����������������Ѿ������򷵻����еģ�����
	  �����µ�
	*/
	T* FetchObj(void)
	{
		while(true)
		{
			if(m_obj_count > 0)
			{
				T* t = static_cast<T*>(*m_objects.begin());
				m_objects.pop_front();
				--m_obj_count;
				t->onEnabledPoolObject();
				return t;
			}

			assignObjs();
		}

		return NULL;
	}

	/*
		����һ������
	*/
	void ReleaseObj(T* obj)
	{
		reclaimObject_(obj);
	}

	/*
		����һ����������
	*/
	void ReleaseObj(std::list<T*>& objs)
	{
		typename std::list<T*>::iterator iter = objs.begin();
		for (; iter != objs.end(); ++iter)
		{
			reclaimObject_((*iter));
		}

		objs.clear();
	}

	/*
		����һ����������
	*/
	void ReleaseObj(std::vector<T*>& objs)
	{
		typename std::vector<T*>::iterator iter = objs.begin();
		for (; iter != objs.end(); ++iter)
		{
			reclaimObject_((*iter));
		}
		objs.clear();
	}

	/*
		����һ����������
	*/
	void ReleaseObj(std::queue<T*>& objs)
	{
		while(!objs.empty())
		{
			T* t = objs.front();
			objs.pop();
			reclaimObject_(t);
		}
	}

	size_t size(void) const {return m_obj_count;}

	std::string c_str()
	{
		char buf[1024];

		sprintf_s(buf, "CObjectPool::c_str(): name=%s, objs=%d/%d, isDestroyed=%s.\n",
			m_name.c_str(), (int)m_obj_count, (int)m_max, (isDestroyed() ? "true" : "false"));
		return buf;
	}
	size_t maxSize() const {return m_max;}
	size_t totalAllocs() const {return m_total_allocs;}
	bool isDestroyed() const {return m_isDestroyed;}

protected:
	/*
		����һ������
	*/
	void reclaimObject_(T* obj)
	{
		if(NULL != obj)
		{
			//������״̬
			obj->onReclaimObject();

			if(size() >= m_max || m_isDestroyed)
			{
				delete obj;
				--m_total_allocs;
			}
			else
			{
				m_objects.push_back(obj);
				++m_obj_count;
			}
		}

		uint64 now_timestamp = getTimeMilliSecond();

		if(m_obj_count <= OBJECT_POOL_INIT_SIZE)
		{
			//С�ڵ�����ˢ�¼��ʱ��
			m_lastReducingCheckTime = now_timestamp;
		}
		else if(m_lastReducingCheckTime - now_timestamp > OBJECT_POOL_REDUCING_TIME_OUT)
		{
			//��ʱ�����OBJECT_POOL_INIT_SIZEδʹ�õĶ�����ʼ��������
			size_t reducing = min(m_objects.size(), min((size_t)OBJECT_POOL_INIT_SIZE, (size_t)(m_obj_count - OBJECT_POOL_INIT_SIZE)));
			while(reducing-- > 0)
			{
				T* t = static_cast<T*>(*m_objects.begin());
				m_objects.pop_front();
				delete t;

				--m_obj_count;
			}
			m_lastReducingCheckTime = now_timestamp;
		}
	}
		 
protected:
	OBJECTS			m_objects;
	size_t			m_max;
	bool			m_isDestroyed;

	std::string		m_name;

	size_t			m_total_allocs;

	//Linux�����У�list.size()ʹ�õ���std::distance(begin(), end())��ʽ���
	//Ӱ������,�����size������¼
	size_t			m_obj_count;

	//���һ��������ʱ��
	//�������OBJECT_POOL_REDUCING_TIME_OUT����OBJECT_POOL_INIT_SIZE,���������OBJECT_POOL_INIT_SIZE
	uint64			m_lastReducingCheckTime;
};


//�ض�������ʹ�óصĶ������ʵ�ֻ��չ���
class PoolObject
{
public:
	virtual ~PoolObject() {}
	virtual void onReclaimObject() = 0;
	virtual void onEnabledPoolObject(){}

	virtual size_t getPoolObjectBytes(){return 0;}

	virtual bool destructorPoolObject()
	{
		return false;
	}
};

template<typename T>
class SmartObjectPool: public CObjectPool<T>
{
public:
};

template<typename T>
class SmartPoolObject
{
public:
	SmartPoolObject(T* pPoolObject, CObjectPool<T>& objectPool)
		:m_pPoolObject(pPoolObject),
			m_objectPool(objectPool)
	{}

	~SmartPoolObject()
	{
		onReclaimObject();	
	}

	void onReclaimObject()
	{
		if(m_pPoolObject != NULL)
		{
			m_objectPool.ReleaseObj(m_pPoolObject);
			m_pPoolObject = NULL;
		}
	}

	T* Get()
	{
		return m_pPoolObject;
	}

	T* operator->()
	{
		return m_pPoolObject;
	}

	T& operator*()
	{
		return *m_pPoolObject;
	}


private:
	T*		m_pPoolObject;
	CObjectPool<T>& m_objectPool;
};

} //namespace sl
#endif
