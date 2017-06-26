//存放Code的队列
/********************************************************************
	created:	2015/12/01
	created:	1:12:2015   17:15
	filename: 	e:\myproject\shyloo\sllib\slcode_queue.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slcode_queue
	file ext:	h
	author:		ddc
	
	purpose:	存放code队列
*********************************************************************/
#ifndef _SL_LIB_CODE_QUEUE_H_
#define _SL_LIB_CODE_QUEUE_H_
#include "sltime.h"
namespace sl
{
///存放Code的队列
/*
	如果这个queue只有两个对象单工使用，那么可以不用互斥
*/
class CCodeQueue
{
private:
	enum {SPACE = 8};
	
public:
	CCodeQueue() {}
	~CCodeQueue() {}

	//初始化Code队列
	inline int Init(int iLen, bool clear){
		if (clear){
			offset_ = sizeof(CCodeQueue);
			size_ = iLen - sizeof(CCodeQueue);

			read_ = 0;
			write_ = 0;
			create_time_ = (unsigned int)sl::getTimeMilliSecond();
			checksum_ = (unsigned int)offset_ ^ (unsigned int)size_ ^ (unsigned int)create_time_;
		}
		return 0;
	}

	inline char* GetBuffer() const {return (char*)this + offset_; }
	inline int GetSize() const {return size_;}
	inline int GetFreeSize() const{ return read_ > write_ ? (read_ - write_ - SPACE - sizeof(int)) : (size_ - write_ + read_ - SPACE - sizeof(int)); }
	inline int GetDataSize() const { return read_ > write_ ? (size_ - read_ + write_) : (write_ - read_); }

	/*
		向队列放入一个Code
		@param [in]  pszBuffer  Code的内存地址
		@param [in]  iLen		Code的内存大小
		@return  0表示成功 -1参数错误  -2缓冲区已满
	*/
	inline int Put(const char* pszBuffer, int iLen){
		if(iLen == 0){
			return 0;
		}
		tempoffset_ = offset_;
		tempsize_   = size_;
		tempread_	= read_;
		tempwrite_  = write_;
		tempcodelen_ = iLen;

		//每次放入的长度最大不能超过0xFFFFFF  = 16M
		if(iLen < 0 || iLen > (size_ - SPACE) || iLen > 0xFFFFFF){
			ECHO_ERROR("put buf to queue failed! len = %d, left len = %d", iLen, size_ - SPACE);
			dump();
			SLASSERT(false, "wtf");
			return -1;
		}

		//newpos表示完成Put后write的位置
		//wrap表示是否换行了
		const int startpos = write_;
		const int minpos   = (read_ < SPACE ? size_ : 0) + read_ - SPACE;
		int newpos   = startpos + iLen;
		int wrap = 0;
		if(newpos >= size_){
			newpos -= size_;
			wrap = 1;
		}

		//判断是否会满
		if(write_ > minpos){
			if(wrap && newpos > minpos){
				//满
				return -2;
			}
		}else{
			if(wrap || newpos > minpos){
				//满
				return -2;
			}
		}

		if(wrap){
			//buf起始位置已经超过了size_
			memcpy(GetBuffer() + startpos, pszBuffer, size_ - startpos);
			memcpy(GetBuffer(), pszBuffer + size_ - startpos, iLen - size_ + startpos);
		}
		else{
			memcpy(GetBuffer() + startpos, pszBuffer, iLen);
		}

		//调整write_指针
		write_	=	newpos;

		return 0;
	}

	/*
		从队列中取出一个code
		@param	[in]  pszBuffer
		@param  [in]  bufLen			传入的是pszBuffer的长度
		@param  [out] ilen				传出的是获取数据的长度
		@return 0正确	-1参数错误	-2缓冲区内容错误
	*/
	inline const char* Get(char* pszBuffer, int readLen, bool isPeek = false){
		if (readLen <= 0){
			return nullptr;
		}
		if(read_ == write_){
			return nullptr;
		}

		const char* retBuf = nullptr;
		const int startpos = read_;
		if (startpos + readLen > size_){
			memcpy(pszBuffer, GetBuffer() + startpos, size_ - startpos);
			memcpy(pszBuffer + size_ - startpos, GetBuffer(), readLen - size_ + startpos);
			retBuf = pszBuffer;
		}
		else{
			retBuf = GetBuffer() + startpos;
		}

		if (!isPeek)
			read_ = (startpos + readLen) % size_;

		return retBuf;
	}

	inline void skip(const int len){
		const int startpos = read_;
		read_ = (startpos + len) % size_;
	}


	//用于在共享内存中创建时做校验
	//@return 0表示CCodeQueue对象已创建， 否则表示对象未创建
	inline int CheckSum() const{
		if(!offset_ || !size_ || !create_time_){
			return -1;
		}
		unsigned int nTempInt = 0;
		nTempInt = (unsigned int)offset_ ^ (unsigned int)size_
				    ^ (unsigned int)create_time_ ^ (unsigned int) checksum_;

		return (nTempInt == 0) ? 0 : -1;
	}

	void dump(){
		ECHO_ERROR("------last offset:[%d]-------", tempoffset_);
		ECHO_ERROR("------last size:[%d]-------", tempsize_);
		ECHO_ERROR("------last read:[%d]-------", tempread_);
		ECHO_ERROR("------last write:[%d]-------", tempwrite_);
		ECHO_ERROR("------last codelen:[%d]-------", tempcodelen_);
	}

protected:
	int			offset_;
	int			size_;
	int			read_;		//已用的数据开始
	int			write_;		//已用数据的结束
		
	//用于服务器崩溃时查看
	int			tempoffset_;
	int			tempsize_;
	int			tempread_;
	int			tempwrite_;
	int			tempcodelen_;

	//////////////////////
	unsigned int	create_time_; //创建时间
	unsigned int	checksum_;	  //校验结果

}; // class CCodeQueue

} //namespace sl

#endif