/* Copyright (C) 
 * 2019 - duandechao
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */
/**
 * @file slstrand_work_queue.h
 * @brief  strand模型的异步工作队列
 * @author duandechao
 * @version 1.0
 * @date 2019-06-20
 */

#ifndef __SL_STRAND_WORK_QUEUE_H__
#define __SL_STRAND_WORK_QUEUE_H__
#include "slcycle_queue.h"
#include <condition_variable>
class SLStrandWorkQueue{
public:
	
	/**
	 * @brief  添加一个任务
	 *
	 * @tparam FUNC
	 * @param  f
	 */
	template <typename FUNC>
	void push(FUNC w){
		std::unique_lock<std::mutex> lock(_mtx);
		_workQueue.push(std::move(w));
		_cond.notify_all();
	}

	void run(){
		//标记当前进程已经注册到workQueue，并且能dispatch
		SLCallStack<SLStrandWorkQueue>::Context ctx(this);
		while(true){
			std::function<void()> work;
			{
				//同步获取工作队列中的任务
				std::unique_lock<std::mutex> lock(_mtx);
				_cond.wait(lock, [this]{return !_workQueue.empty();});
				work = std::move(_workQueue.front());
				_workQueue.pop();
			}

			if(work){
				work();
			}
            else {
				//当获取的任务为nullptr时表示退出
				//这个时候再push一个nullptr让其他线程退出
                push(nullptr);
                return;
            }
		}
	}

	inline void stop() { push(nullptr);}

	inline bool canDispatch() {
		return SLCallStack<SLStrandWorkQueue>::contain(this) != NULL;
	}

private:
	//控制线程同步获取任务
	std::condition_variable				_cond;
	std::mutex							_mtx;

	//待处理任务队列
	std::queue<std::function<void()>>	_waitQueue;

	//处理完成的任务队列
	std::queue<std::function<void()>>	_completeQueue;
};
#endif
