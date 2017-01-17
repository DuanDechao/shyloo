#ifndef __SL_LIB_THREAD_POOL_H__
#define __SL_LIB_THREAD_POOL_H__
#include "slmulti_sys.h"
#include <queue>
#include <list>
#define THREAD_BUSY_SIZE 32

class TPTask;
class TPThread;
class SLThreadPool{
public:
	SLThreadPool(int32 newThreadCount, int32 defaultThreadCount, int32 maxThreadCount);

	//��ȡ��ǰ�߳�����
	int32 currentThreadCount(void) const { return m_currThreadCount; }
	//��ȡ��ǰ�����߳�����
	int32 currentFreeThreadCount(void) const { return m_currFreeThreadCount; }
	//�̳߳��Ƿ��ڷ�æ״̬
	bool isBusy(void) const { return m_bufferedTaskList.size() > THREAD_BUSY_SIZE; }
	//��û������������
	int32 bufferTaskSize() const { return (int32)m_bufferedTaskList.size(); }
	//����Ѿ��Ѿ������������
	int32 finiTaskSize() const { return (int32)m_finiTaskListCount; }


	//����һ���̳߳��߳�
	TPThread* createThread(int32 threadWaitSecond);

	//���̳߳����һ������
	bool addTask(TPTask* tpTask);

	//��ĳ�����񱣴浽δ�����б�
	void bufferTask(TPTask* tpTask);

	//��δ�����б�ȡ��һ������
	TPTask* popBufferTask(void);

	//�ƶ�һ���̵߳������б�
	bool addFreeThread(TPThread* td);

	//�ƶ�һ���̵߳���æ�б�
	bool addBusyThread(TPThread* td);

	//���һ���Ѿ���ɵ������б�
	void addFiniTask(TPTask* pTask);

	//ɾ��һ�����𣨳�ʱ�����߳�
	bool removeHangThread(TPThread* td);

	void onMainThreadTick();

	bool isThreadCountMax(void) const { return m_currThreadCount >= m_maxThreadCount; }
private:
	bool					m_isInitialize;				//�̳߳��Ƿ��Ѿ���ʼ��
	bool					m_isDestroyed;				//�Ƿ�����

	std::queue<TPTask*>		m_bufferedTaskList;			//�ȴ�����������б�
	std::list<TPTask*>		m_finiTaskList;				//�Ѿ�������������б�
	int32					m_finiTaskListCount;
	
	CRITICAL_SECTION		m_bufferedTaskListMutex;	//�ȴ����������б�Ļ�����
	CRITICAL_SECTION		m_threadStateListMutex;		//�л��߳�״̬�Ļ�����
	CRITICAL_SECTION		m_finiTaskListMutex;		//������������б�Ļ�����

	std::list<TPThread*>    m_busyThreadList;			//��æ���߳��б�
	std::list<TPThread*>	m_freeThreadList;			//���е��߳��б�
	std::list<TPThread*>	m_allThreadList;			//���е��߳��б�

	int32					m_maxThreadCount;			//����߳���
	int32					m_extraNewAddThreadCount;	//�̱߳���ʱ��һ�����ӵ��߳���
	int32					m_currThreadCount;			//��ǰ�߳���
	int32					m_currFreeThreadCount;		//��ǰ���е��߳���
	int32					m_defaultThreadCount;		//��ʼĬ�Ͽ������߳���
};
#endif