#ifndef __SL_LIB_THREAD_POOL_H__
#define __SL_LIB_THREAD_POOL_H__
#include "slmulti_sys.h"
#include "slthread.h"
#include <queue>
#include <list>
namespace sl{
namespace thread
{
#define THREAD_BUSY_SIZE 32

class TPThread;
class SLThreadPool : public ISLThreadPool{
public:
	SLThreadPool(int32 newThreadCount, int32 defaultThreadCount, int32 maxThreadCount);
	~SLThreadPool();

	//��ȡ��ǰ�߳�����
	virtual int32 SLAPI currentThreadCount(void) const { return m_currThreadCount; }

	//��ȡ��ǰ�����߳�����
	virtual int32 SLAPI currentFreeThreadCount(void) const { return m_currFreeThreadCount; }

	//�̳߳��Ƿ��ڷ�æ״̬
	virtual bool SLAPI isBusy(void) const { return m_bufferedTaskList.size() > THREAD_BUSY_SIZE; }

	//��û������������
	virtual int32 SLAPI bufferTaskSize() const { return (int32)m_bufferedTaskList.size(); }

	//����Ѿ��Ѿ������������
	virtual int32 SLAPI finiTaskSize() const { return (int32)m_finiTaskListCount; }

	virtual bool SLAPI isDestroyed(void) const { return m_isDestroyed; }

	virtual void SLAPI release();

	//����
	virtual bool SLAPI start();

	//���̳߳����һ������
	virtual bool SLAPI addTask(ITPTask* pTask);

	//run
	virtual bool SLAPI run(int64 overTime);

	virtual bool mainThreadTick();

	bool isThreadCountMax(void) const { return m_currThreadCount >= m_maxThreadCount; }

	//����һ���̳߳��߳�
	TPThread* createThread(int32 threadWaitSecond);
	
	//��ĳ�����񱣴浽δ�����б�
	void bufferTask(ITPTask* pTask);

	//��δ�����б�ȡ��һ������
	ITPTask* popBufferTask(void);

	//�ƶ�һ���̵߳������б�
	bool addFreeThread(TPThread* td);

	//�ƶ�һ���̵߳���æ�б�
	bool addBusyThread(TPThread* td);

	//���һ���Ѿ���ɵ������б�
	void addFiniTask(ITPTask* pTask);

	//ɾ��һ�����𣨳�ʱ�����߳�
	bool removeHangThread(TPThread* td);

	bool hasThread(TPThread* td);

private:
	void destroy();

private:
	bool					m_isInitialize;				//�̳߳��Ƿ��Ѿ���ʼ��
	bool					m_isDestroyed;				//�Ƿ�����

	std::queue<ITPTask*>	m_bufferedTaskList;			//�ȴ�����������б�
	std::list<ITPTask*>		m_finiTaskList;				//�Ѿ�������������б�
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
}
}

#endif