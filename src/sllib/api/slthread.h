#ifndef __SL_LIB_SLTHREAD_H__
#define __SL_LIB_SLTHREAD_H__
#include "sltype.h"

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
	virtual void SLAPI release() = 0;
};

class ISLThreadPool{
public:
	//��ȡ��ǰ�߳�����
	virtual int32 SLAPI currentThreadCount(void) const = 0;

	//��ȡ��ǰ�����߳�����
	virtual int32 SLAPI currentFreeThreadCount(void) const = 0;
	
	//�̳߳��Ƿ��ڷ�æ״̬
	virtual bool SLAPI isBusy(void) const = 0;
	
	//��û������������
	virtual int32 SLAPI bufferTaskSize() const = 0;
	
	//����Ѿ��Ѿ������������
	virtual int32 SLAPI finiTaskSize() const = 0;

	virtual bool SLAPI isDestroyed(void) const = 0;

	//����
	virtual bool SLAPI start() = 0;

	//���̳߳����һ������
	virtual bool SLAPI addTask(ITPTask* pTask) = 0;

	//���߳�ִ��
	virtual bool SLAPI run(int64 overTime) = 0;

	//�ͷ�
	virtual void SLAPI release() = 0;
};

ISLThreadPool* SLAPI createThreadPool(int32 newThreadCount, int32 defaultThreadCount, int32 maxThreadCount);

}
}
#endif