#ifndef __SL_LIB_SLTHREAD_H__
#define __SL_LIB_SLTHREAD_H__
#include "slmulti_sys.h"

namespace sl
{
namespace thread
{
enum TPTaskState{
	//�������
	TPTASK_STATE_COMPLETED,

	//��������������߳�ִ��
	TPTASK_STATE_CONTINUE_MAINTHREAD,

	//��������������߳�ִ��
	TPTASK_STATE_CONTINUE_CHILDTHREAD,
};

class ITPTask{
public:
	virtual bool SLAPI start() = 0;
	virtual bool SLAPI process() = 0;
	virtual bool SLAPI end() = 0;
	virtual TPTaskState SLAPI presentMainThread() = 0;
};

class ISLThreadPool{
public:
	//��ȡ��ǰ�߳�����
	virtual int32 SLAPI currentThreadCount(void) const;

	//��ȡ��ǰ�����߳�����
	virtual int32 SLAPI currentFreeThreadCount(void) const;
	
	//�̳߳��Ƿ��ڷ�æ״̬
	virtual bool SLAPI isBusy(void) const;
	
	//��û������������
	virtual int32 SLAPI bufferTaskSize() const;
	
	//����Ѿ��Ѿ������������
	virtual int32 SLAPI finiTaskSize() const;

	virtual bool SLAPI isDestroyed(void) const;

	//����
	virtual bool SLAPI start();

	//���̳߳����һ������
	virtual bool SLAPI addTask(ITPTask* pTask);

	//���߳�ִ��
	virtual void SLAPI onMainThreadTick();

	//�ͷ�
	virtual void SLAPI release();
};

ISLThreadPool* SLAPI createThreadPool(int32 newThreadCount, int32 defaultThreadCount, int32 maxThreadCount);

}
}
#endif