/********************************************************************
	created:	2016/08/06
	created:	6:8:2016   12:51
	filename: 	c:\Users\ddc\Desktop\shyloo\libs\common\sltasks.h
	file path:	c:\Users\ddc\Desktop\shyloo\libs\common
	file base:	sltasks
	file ext:	h
	author:		ddc
	
	purpose:	
*********************************************************************/

#ifndef _SL_TASKS_H_
#define _SL_TASKS_H_
#include "slbase.h"
#include "sltask.h"
namespace sl
{
//任务管理器
class Tasks
{
public:
	Tasks();
	~Tasks();

	void add(Task* pTask);
	bool cancel(Task* pTask);
	void process();
private:
	typedef std::vector<sl::Task *> Container;
	Container m_container;
};
}
#endif