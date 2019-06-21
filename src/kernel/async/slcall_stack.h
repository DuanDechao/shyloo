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
 * @file slcall_stack.h
 * @brief  用于记录线程调用栈信息
 * @author duandechao
 * @version 1.0
 * @date 2019-06-20
 */

#ifndef __SL_CALL_STACK_H__
#define __SL_CALL_STACK_H__

#include "slmulti_sys.h"
#include <thread>

template<typename KEY, typename VALUE = uint8>
class SLCallStack{
public:
	
	//记录调用的上下文
	class Context{
	public:
		//禁用拷贝构造函数和赋值构造函数
		Context(const Context&) = delete;
		Context& operator=(const Context*) = delete;

		Context(KEY* key)
			:_key(key),
			 _next(SLCallStack<KEY, VALUE>::_stackCallTop)
		{
			_value = reinterpret_cast<uint8*>(this);
			SLCallStack<KEY, VALUE>::_stackCallTop = this;
		}

		Context(KEY* key, VALUE* value)
			:_key(key), 
			 _value(value),
		 	 _next(SLCallStack<KEY, VALUE>::_stackCallTop)
		{
			SLCallStack<KEY, VALUE>::_StackCallTop = this;
		}
		
		/**
		 * 析构时删除当前的调用栈信息记录
		 */
		~Context(){
			SLCallStack<KEY, VALUE>::_stackCallTop  = _next;
		}
		
		inline KEY* key() {return _key;}
		inline VALUE* value() {return _value;}
		inline Context* next() {return _next;}

	private:
		KEY*		_key;
		VALUE*		_value;

		//通过_next指针记录调用栈信息
		Context*	_next;
	};

	/**
	 * @brief 判断key值是否被当前调用栈调用  
	 *
	 * @param  key
	 *
	 * @return  
	 */
	static VALUE* contain(const KEY* key){
		Context* context = _stackCallTop;
		while(context){
			if(context->key() == key)
				return context->value();
			context = context->next();
		}
		return nullptr;
	}

	static inline Context* top(){return _stackCallTop;}

private:
	static thread_local Context*	_stackCallTop;
};

template <typename KEY, typename VALUE>
thread_local typename SLCallStack<KEY, VALUE>::Context*
	SLCallStack<KEY, VALUE>::_stackCallTop = nullptr;

#endif
