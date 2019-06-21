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
 * @file slstrand.h
 * @brief  strand
 * @author duandechao
 * @version 1.0
 * @date 2019-06-20
 */

#ifndef _SL_STRAND_H_
#define _SL_STRAND_H_

#include "slmulti_sys.h"
#include <queue>
#include "slsync_monitor.h"
#include "slcall_stack.h"
#include "slstrand_work_queue.h"

class SLStrand{
public:
	SLStrand(SLStrandWorkQueue* queue):_workQueue(queue){}
	SLStrand(const SLStrand&) = delete;
	SLStrand& operator=(const SLStrand&) = delete;

	/**
	 * @brief
	 * 如果线程正在执行当前strand，则直接处理work
	 * 否则，加入strand的待处理队列，由处理当前
	 * strand的线程后续处理
	 *
	 * @tparam FUNC
	 * @param  work
	 *
	 * @return  
	 */
	template<typename FUNC>
	void dispatch(FUNC work){
		//判断当前的线程是否进入可处理任务状态
		if(!_workQueue->canDispatch()){
			SLASSERT(false, "wtf");
			return;
		}

		//如果当前线程正在处理这个strand，就立即执行
		//这时候能够保证没有其他线程执行此strand的任务
		if(runningInThisThread()){
			work();
			return;
		}

		//到此处可以判断当前线程没有在处理此Strand
		//这时候判断strand是否有被其他线程处理，
		//如果没有，则拿来执行，反之,则post到待处理队列
		auto trigger = _localData([&](StrandLocal& data){
			if(data._running){
				data._waitQueue.push(std::move(work));
				return false;
			}
			else {
				data._running = true;
				return true;
			}
		});

		if(trigger){
			//如果没有其他线程处理这个strand
			//记录当前strand到调用栈
			//并立即执行
			SLCallStack<SLStrand>::Context ctx(this);
			work();

			//这里需要调用run
			//比如在前面work期间有其他线程向waitqueue中append work
			//这时需要持续处理waitqueue中的任务
			run();

		}
	}

	/**
	 * @brief  判断当前线程是否正在处理这个strand
	 *
	 * @return  
	 */
	inline bool runningInThisThread(){
		return SLCallStack<SLStrand>::contain(this) != nullptr;
	}

private:
	//处理等待队列中的任务
	//此时能够确保当前线程正在处理这个strand
	//并且strand处于running状态
	//处理完将running设为false
	void run(){
		SLCallStack<SLStrand>::Context ctx(this);
		while(true){
			std::function<void()> work;
			_localData([&](StrandLocal& data){
				SLASSERT(data._running, "wtf");
				if(data._waitQueue.size()){
					work = std::move(data._waitQueue.front());
					data._waitQueue.pop();
				}
				else{
					data._running = false;
				}
			});

			if(work){
				work();
			}
			else 
				return;
		}
	}

private:
	//Strand本地数据
	struct StrandLocal{
		//strand当前是否被某个线程执行
		bool	_running = false;		

		//待处理任务队列
		std::queue<std::function<void()>> _waitQueue;		
	};

private:
	SLStrandWorkQueue*			_workQueue;
	SLSyncMonitor<StrandLocal>	_localData;
};
#endif

