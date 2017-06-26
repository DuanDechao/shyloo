//���Code�Ķ���
/********************************************************************
	created:	2015/12/01
	created:	1:12:2015   17:15
	filename: 	e:\myproject\shyloo\sllib\slcode_queue.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slcode_queue
	file ext:	h
	author:		ddc
	
	purpose:	���code����
*********************************************************************/
#ifndef _SL_LIB_CODE_QUEUE_H_
#define _SL_LIB_CODE_QUEUE_H_
#include "sltime.h"
namespace sl
{
///���Code�Ķ���
/*
	������queueֻ���������󵥹�ʹ�ã���ô���Բ��û���
*/
class CCodeQueue
{
private:
	enum {SPACE = 8};
	
public:
	CCodeQueue() {}
	~CCodeQueue() {}

	//��ʼ��Code����
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
		����з���һ��Code
		@param [in]  pszBuffer  Code���ڴ��ַ
		@param [in]  iLen		Code���ڴ��С
		@return  0��ʾ�ɹ� -1��������  -2����������
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

		//ÿ�η���ĳ�������ܳ���0xFFFFFF  = 16M
		if(iLen < 0 || iLen > (size_ - SPACE) || iLen > 0xFFFFFF){
			ECHO_ERROR("put buf to queue failed! len = %d, left len = %d", iLen, size_ - SPACE);
			dump();
			SLASSERT(false, "wtf");
			return -1;
		}

		//newpos��ʾ���Put��write��λ��
		//wrap��ʾ�Ƿ�����
		const int startpos = write_;
		const int minpos   = (read_ < SPACE ? size_ : 0) + read_ - SPACE;
		int newpos   = startpos + iLen;
		int wrap = 0;
		if(newpos >= size_){
			newpos -= size_;
			wrap = 1;
		}

		//�ж��Ƿ����
		if(write_ > minpos){
			if(wrap && newpos > minpos){
				//��
				return -2;
			}
		}else{
			if(wrap || newpos > minpos){
				//��
				return -2;
			}
		}

		if(wrap){
			//buf��ʼλ���Ѿ�������size_
			memcpy(GetBuffer() + startpos, pszBuffer, size_ - startpos);
			memcpy(GetBuffer(), pszBuffer + size_ - startpos, iLen - size_ + startpos);
		}
		else{
			memcpy(GetBuffer() + startpos, pszBuffer, iLen);
		}

		//����write_ָ��
		write_	=	newpos;

		return 0;
	}

	/*
		�Ӷ�����ȡ��һ��code
		@param	[in]  pszBuffer
		@param  [in]  bufLen			�������pszBuffer�ĳ���
		@param  [out] ilen				�������ǻ�ȡ���ݵĳ���
		@return 0��ȷ	-1��������	-2���������ݴ���
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


	//�����ڹ����ڴ��д���ʱ��У��
	//@return 0��ʾCCodeQueue�����Ѵ����� �����ʾ����δ����
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
	int			read_;		//���õ����ݿ�ʼ
	int			write_;		//�������ݵĽ���
		
	//���ڷ���������ʱ�鿴
	int			tempoffset_;
	int			tempsize_;
	int			tempread_;
	int			tempwrite_;
	int			tempcodelen_;

	//////////////////////
	unsigned int	create_time_; //����ʱ��
	unsigned int	checksum_;	  //У����

}; // class CCodeQueue

} //namespace sl

#endif