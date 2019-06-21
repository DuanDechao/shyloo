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
 * @file slsync_monitor.h
 * @brief 监控对数据T的同步操作 
 * @author duandechao
 * @version 1.0
 * @date 2019-06-21
 */

#ifndef __SL_SYNC_MONITOR_H__
#define __SL_SYNC_MONITOR_H__

#include <mutex>
template <class T>
class SLSyncMonitor{
private:
	mutable T			_data;
	mutable	std::mutex	_mtx;

public:
	SLSyncMonitor(){}
	SLSyncMonitor(T _t): _data(std::move(_t)) {}


	/**
	 * @brief  多线程同步对_data的f操作
	 *
	 * @tparam FUNC
	 * @param  f(_data)
	 *
	 * @return  
	 */
	template <typename FUNC>
	auto operator()(FUNC f) const -> decltype(f(_data)){
		std::lock_guard<std::mutex> lock(_mtx);
		return f(_data);
	}

};

#endif


