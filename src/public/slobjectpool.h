/********************************************************************
	created:	2016/08/03
	created:	3:8:2016   13:42
	filename: 	d:\Projects\shyloo\libs\common\slobjectpool.h
	file path:	d:\Projects\shyloo\libs\common
	file base:	slobjectpool
	file ext:	h
	author:		ddc
	
	purpose:    һЩ�����Ƶ���Ĵ������磺MemoeryStream��Bundle��TCPPacket�ȵ�
				�������ض�ͨ����������ֵ��Ч��Ԥ����ǰ��������һЩ���󻺴��������õ�ʱֱ��
				�ڶ�����л�ȡһ��δ��ʹ�õĶ��󼴿�
******************************************************************/

#ifndef _SL_OBJECT_POOL_H_
#define _SL_OBJECT_POOL_H_

#include "slmulti_sys.h"
#include "sltime.h"
#include <set>
#include <string>
#include <queue>
#include <list>
#include <map>

namespace sl
{
#define OBJECT_POOL_INIT_SIZE			16
#define OBJECT_POOL_INIT_MAX_SIZE		OBJECT_POOL_INIT_SIZE * 1024

//ÿ5���Ӽ��һ������
#define OBJECT_POOL_REDUCING_TIME_OUT	5 * MINUTE

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

	~CObjectPool()
	{
		destroy();
	}

	uint32 maxSize() const {return m_max;}
	uint32 totalAllocs() const {return m_total_allocs;}
	bool isDestroyed() const {return m_isDestroyed;}
	uint32 size(void) const {return m_obj_count;}

public:

	void destroy()
	{
		m_isDestroyed = true;
		typename OBJECTS::iterator iter = m_objects.begin();
		for (; iter != m_objects.end(); ++iter)
		{
			delete (*iter);
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
	  ����һ����������������Ѿ������򷵻����еģ�����
	  �����µ�
	*/
	template<typename... Args>
	T* fetchObj(Args... args)
	{
		while(true)
		{
			if(m_obj_count > 0)
			{
				T* t = static_cast<T*>(*m_objects.begin());
				m_objects.pop_front();
				--m_obj_count;
				return new(t)T(args...);
			}
			assignObjs();
		}

		return NULL;
	}

	/*
		����һ������
	*/
	void releaseObj(T* obj)
	{
		reclaimObject_(obj);
	}

	std::string c_str()
	{
		char buf[1024];

		sprintf_s(buf, "CObjectPool::c_str(): name=%s, objs=%d/%d, isDestroyed=%s.\n",
			m_name.c_str(), m_obj_count, m_max, (isDestroyed() ? "true" : "false"));
		return buf;
	}
	
protected:
	/*
		����һ������
	*/
	void reclaimObject_(T* obj)
	{
		if(NULL != obj)
		{
			//������״̬
			obj->~T();

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
			uint32 reducing = min((uint32)m_objects.size(), min((uint32)OBJECT_POOL_INIT_SIZE, 
				(uint32)(m_obj_count - OBJECT_POOL_INIT_SIZE)));

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
	uint32			m_max;
	bool			m_isDestroyed;
	std::string		m_name;
	uint32			m_total_allocs;

	//Linux�����У�list.size()ʹ�õ���std::distance(begin(), end())��ʽ���
	//Ӱ������,�����size������¼
	uint32			m_obj_count;

	//���һ��������ʱ��
	//�������OBJECT_POOL_REDUCING_TIME_OUT����OBJECT_POOL_INIT_SIZE,���������OBJECT_POOL_INIT_SIZE
	uint64			m_lastReducingCheckTime;
};

#define CREATE_OBJECT_POOL(CLASSNAME) \
	static sl::CObjectPool<CLASSNAME> g_objPool##CLASSNAME("obj"#CLASSNAME)

#define CREATE_POOL_OBJECT(CLASSNAME, ...) \
	g_objPool##CLASSNAME.fetchObj(__VA_ARGS__)
	
#define RELEASE_POOL_OBJECT(CLASSNAME, OBJECT) \
	g_objPool##CLASSNAME.releaseObj(OBJECT)

#define DESTROY_OBJECT_POOL(CLASSNAME)	\
	g_objPool##CLASSNAME.destroy()

} //namespace sl
#endif
