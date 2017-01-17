#ifndef __SL_LIB_TPTHREAD_H__
#define __SL_LIB_TPTHREAD_H__
#include "slmulti_sys.h"
class SLThreadPool;
class TPTask;
class TPThread{
public:
	enum THREAD_STATE{
		THREAD_STATE_STOP = -1,
		THREAD_STATE_SLEEP = 0,
		THREAD_STATE_BUSY = 1,
		THREAD_STATE_END = 2
	};

public:
	TPThread(SLThreadPool* threadPool, int32 threadWaitSecond = 0)
		:m_threadWaitSecond(threadWaitSecond),
		m_currTask(nullptr),
		m_threadPool(threadPool)
	{
		m_state = THREAD_STATE_SLEEP;
		initCond();
		intMutex();
	}

	~TPThread(){
		deleteCond();
		deleteMutex();
	}

	inline HANDLE id() const { return m_threadId; }
	inline void setId(HANDLE tid) { m_threadId = tid; }

	HANDLE createThread(void);

	//��ñ��߳�Ҫ���������
	inline TPTask* task(void) const{ return m_currTask; }

	//���ñ��߳�Ҫ���������
	inline void setTask(TPTask* pTask){ m_currTask = pTask; }

	//��ǰ״̬
	inline int32 state(void) const { return m_state; }

	//��ǰ���������
	void onTaskCompleted(void);

	//�߳�֪ͨ �ȴ������ź�
	bool onWaitCondSignal(void);

	bool join(void);

	TPTask* tryGetTask(void);

	//���³���ִ�е�������
	inline void resetDoneTasks() { m_doneTasks = 0; }
	inline void incDoneTasks(){ ++m_doneTasks; }

#ifdef SL_OS_WINDOWS
	static unsigned __stdcall threadFunc(void *args);
#else
	static void* threadFunc(void* args);
#endif
	
	inline void initCond(void){
		m_cond = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	inline void intMutex(void){
		InitializeCriticalSection(&m_mutex);
	}

	inline void deleteCond(void){
		CloseHandle(m_cond);
	}

	inline void deleteMutex(void){
		DeleteCriticalSection(&m_mutex);
	}

	inline void lock(void){
		EnterCriticalSection(&m_mutex);
	}

	inline void unLock(void){
		LeaveCriticalSection(&m_mutex);
	}

	inline int32 sendCondSignal(void){
		return SetEvent(m_cond);
	}

private:
	HANDLE					m_cond;				//�߳��ź���
	CRITICAL_SECTION		m_mutex;			//�̻߳�����
	int32					m_threadWaitSecond;	//�߳̿�����ȴ�ʱ�䣬������ɾ��
	TPTask*					m_currTask;			//�̵߳�ǰִ�е�����
	HANDLE					m_threadId;			//�߳�ID
	SLThreadPool*			m_threadPool;		//�����̳߳�
	THREAD_STATE			m_state;			//�߳�״̬ -1��ʾ��δ����
	int32					m_doneTasks;		//�̳߳���ִ����������
};
#endif