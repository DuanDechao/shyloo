/********************************************************************
	created:	2016/08/06
	created:	6:8:2016   12:48
	filename: 	c:\Users\ddc\Desktop\shyloo\libs\common\sltask.h
	file path:	c:\Users\ddc\Desktop\shyloo\libs\common
	file base:	sltask
	file ext:	h
	author:		ddc
	
	purpose:	
*********************************************************************/

#ifndef _SL_TASK_H_
#define _SL_TASK_H_
namespace sl
{
//抽象一个任务
class Task
{
public:
	virtual ~Task() {}
	virtual bool process() = 0;
};
}
#endif