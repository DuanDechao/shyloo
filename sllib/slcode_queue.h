//存放Code的队列

#ifndef _SL_CODE_QUEUE_H_
#define _SL_CODE_QUEUE_H_
#include <string.h>
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
		int Init(int iLen)
		{
			offset_		  =		sizeof(CCodeQueue);
			size_		  =		iLen - sizeof(CCodeQueue);
			read_		  =		0;
			write_	 	  =		0;
			create_time_  =     (unsigned int) time(0);
			
			*(char*) GetBuffer() = 0;

			checksum_	  =     (unsigned int) offset_ ^(unsigned int) size_ ^ (unsigned int) create_time_;
			return 0;
		}

		char* GetBuffer() const {return (char*)this + offset_; }
		int GetSize() const {return size_;}
		int GetFreeSize() const
		{
			if(read_ > write_)
			{
				return read_ - write_ -SPACE - sizeof(int);
			}
			else
			{
				return size_ - write_ + read_- SPACE -sizeof(int);
			}
		}

		/*
			向队列放入一个Code
			@param [in]  pszBuffer  Code的内存地址
			@param [in]  iLen		Code的内存大小
			@return  0表示成功 -1参数错误  -2缓冲区已满
		*/
		int Put(const char* pszBuffer, int iLen)
		{
			if(iLen == 0)
			{
				return 0;
			}
			tempoffset_ = offset_;
			tempsize_   = size_;
			tempread_	= read_;
			tempwrite_  = write_;

			//每次放入的长度最大不能超过0xFFFFFF  = 16M
			if(iLen < 0 || iLen > (size_ - SPACE - (int)sizeof(int)) || iLen > 0xFFFFFF)
			{
				return -1;
			}

			//newpos表示完成Put后write的位置
			//wrap表示是否换行了
			const int startpos = write_ + sizeof(int);
			const int minpos   = (read_ < SPACE ? size_ : 0) + read_ - SPACE;
			int newpos   = startpos + iLen;
			int wrap = 0;
			if(newpos >= size_)
			{
				newpos -= size_;
				wrap = 1;
			}

			//判断是否会满
			if(write_ > minpos)
			{
				if(wrap && newpos > minpos) //满
				{
					return -2;
				}
			}else
			{
				if(wrap || newpos > minpos) //满
				{
					return -2;
				}
			}

			if(wrap)
			{
				//buf起始位置已经超过了size_
				if(startpos >= size_)
				{
					memcpy(GetBuffer() + startpos -size_, pszBuffer, iLen);
				}
				else
				{
					memcpy(GetBuffer() + startpos, pszBuffer, size_-startpos);
					memcpy(GetBuffer(), pszBuffer, iLen - size_ + startpos);
				}
			}
			else
			{
				memcpy(GetBuffer() + startpos, pszBuffer, iLen);
			}

			//预先把下一个头部长度设为0
			*(char*)(GetBuffer() + newpos)  =  0;

			//调整write_指针
			wrap	=	write_;
			write_	=	newpos;
			newpos  =   write_;

			//把长度写到头部
			iLen	=	ntohl(iLen);
			if(size_ - newpos < (int) sizeof(int))
			{
				memcpy(GetBuffer() + newpos, &iLen, size_ - newpos);
				memcpy(GetBuffer(), ((char*)&iLen) + size_ - newpos, sizeof(int) -size_ + newpos);
			}
			else
			{
				memcpy(GetBuffer() + newpos, &iLen, sizeof(int));
			}
			*(char*)(GetBuffer() + newpos) = 1;
			
			tempcodelen_ = iLen;

			return 0;
		}

		/*
			把两块缓冲区合并成一个Code放入队列中
			这个函数的作用是为了减少一次内存拷贝
			@param [in] pszBuffer
			@param [in] iLen
			@return 0正确 -1参数错误  -2缓冲区满
		*/
		int Put(const char* pszBuffer1, int iLen1, const char* pszBuffer2, int iLen2)
		{
			if(pszBuffer1 == NULL || pszBuffer2 == NULL || iLen2 <= 0 || iLen1 <= 0)
			{
				return -1;
			}

			//每次放入的长度最大不能超过0xFFFFFF  = 16M
			int iLen = iLen1 + iLen2;
			if(iLen > (size_ - SPACE - (int)sizeof(int)) || iLen > 0xFFFFFF)
			{
				return -1;
			}

			//newpos是完成Put后write_的位置
			//wrap表示是否换行了
			//minpos是本次write的极限
			const int startpos = write_ + (int)sizeof(int);
			const int minpos   = (read_ < SPACE ? size_ : 0) + read_ - SPACE;
			int newpos = startpos + iLen;
			int wrap = 0;
			if(newpos >= size_)
			{
				newpos -= size_;
				wrap = 1;
			}

			//判断是否会满
			if(write_ > minpos)
			{
				if(wrap && newpos > minpos)
				{
					return -2;
				}
			}
			else
			{
				if(wrap || newpos > minpos)
				{
					return -2;
				}
			}

			if(wrap)
			{
				//buf起始位置已经超过了size_
				if(startpos >= size_)
				{
					memcpy(GetBuffer() + startpos - size_, pszBuffer1, iLen1);
					memcpy(GetBuffer() + startpos -size_ + iLen1, pszBuffer2, iLen2);
				}
				else
				{
					const int iLeft = size_ - startpos;
					if(iLeft >= iLen1)
					{
						memcpy(GetBuffer() + startpos, pszBuffer1, iLen1);
						memcpy(GetBuffer() + startpos + iLen1, pszBuffer2, iLeft - iLen1);
						memcpy(GetBuffer(), pszBuffer2 + iLeft - iLen1, iLen2 - iLeft + iLen1);
					}
					else
					{
						memcpy(GetBuffer() + startpos, pszBuffer1, iLeft);
						memcpy(GetBuffer(), pszBuffer1 + iLeft, iLen1 - iLeft);
						memcpy(GetBuffer() + iLen1 - iLeft, pszBuffer2, iLen2);
					}
				}

			}
			else
			{
				memcpy(GetBuffer() + startpos, pszBuffer1, iLen1);
				memcpy(GetBuffer() + startpos + iLen1, pszBuffer2, iLen2);
			}

			//预先把下一个头部长度设为0
			*(char*)(GetBuffer() + newpos) = 0;

			//调整write_指针
			wrap = write_;
			write_ = newpos;
			newpos = wrap;

			//把长度写到头部
			iLen = ntohl(iLen);
			if(size_ - newpos < (int)sizeof(int))
			{
				memcpy(GetBuffer() + newpos, &iLen, size_ - newpos);
				memcpy(GetBuffer(), ((char*)&iLen) + size_- newpos, sizeof(int) - size_ + newpos);
			}
			else
			{
				memcpy(GetBuffer() + newpos, &iLen, sizeof(int));
			}
			*(char*)(GetBuffer() + newpos) = 1;
			return 0;

		}

		/*
			从队列中取出一个code
			@param	[in]  pszBuffer
			@param  [in]  bufLen			传入的是pszBuffer的长度
			@param  [out] ilen				传出的是获取数据的长度
			@return 0正确	-1参数错误	-2缓冲区内容错误
		*/
		int Get(char* pszBuffer, int ibufLen, int& iLen)
		{
			if(ibufLen <= 0)
			{
				return -1;
			}
			if(read_ == write_)
			{
				iLen = 0;
				return 0;
			}

			int readlen = 0;
			const int startpos = read_ + sizeof(int);

			char flag = *(char*) (GetBuffer() + read_);
			if(flag != 1)
			{
				iLen = 0;
				return 0;
			}

			//获取要读取的数据长度
			if(startpos > size_)
			{
				memcpy(&readlen, GetBuffer() + read_, size_ - read_);
				memcpy( ((char*)&readlen) + size_ - read_, GetBuffer(), startpos - size_ );
			}
			else
			{
				memcpy(&readlen, GetBuffer() + read_, sizeof(int));
			}
			readlen = htonl(readlen);
			readlen = readlen & 0x00FFFFFF;

			if(readlen == 0)
			{
				iLen = 0;
				return 0;
			}

			if(readlen > ibufLen || readlen < 0)
			{
				return -2;
			}

			if(startpos >= size_)
			{
				memcpy(pszBuffer, GetBuffer() + startpos - size_, readlen);
			}
			else
			{
				if(startpos + readlen > size_)
				{
					memcpy(pszBuffer, GetBuffer() + startpos, size_ - startpos);
					memcpy(pszBuffer + size_ - startpos, GetBuffer(), readlen - size_ + startpos);
				}
				else
				{
					memcpy(pszBuffer, GetBuffer() + startpos, readlen);
				}
			}
			iLen = readlen;
			read_ = (startpos + readlen) % size_;

			return 0;
		}

		/*
			返回最近一个Code的长度
		*/
		int GetCodeLen() const
		{
			if(read_ == write_)
			{
				return 0;
			}

			int readlen = 0;
			const int startpos = read_ + sizeof(int);

			char flag = *(char*)(GetBuffer() + startpos);
			if(flag != 1)
			{
				return 0;
			}

			//获取要读取的数据长度
			if(startpos > size_)
			{
				memcpy(&readlen, GetBuffer() + read_, size_ -read_);
				memcpy(((char*)&readlen) + size_ - read_, GetBuffer(), startpos - size_);
			}
			else
			{
				memcpy(&readlen, GetBuffer() + read_, sizeof(int));
			}
			
			return htonl(readlen) & 0x00FFFFFF;
		}

		//删除一个Code
		int Remove()
		{
			int readlen = 0;
			const int startpos = read_ + sizeof(int);

			if(read_ == write_)
			{
				return 0;
			}

			char flag = *(char*)(GetBuffer() + read_);
			if(flag != 1)
			{
				return 0;
			}

			//获取要读取的数据长度
			if(startpos > size_)
			{
				memcpy(&readlen, GetBuffer() + read_, size_ - read_);
				memcpy( ((char*)&readlen) + size_ - read_, GetBuffer(), startpos - size_);
			}
			else
			{
				memcpy(&readlen, GetBuffer() + read_, sizeof(int));
			}
			readlen = htonl(readlen);
			readlen = readlen & 0x00FFFFFF;

			if(readlen < 0)
			{
				return -2;
			}
			
			read_ = (startpos + readlen) % size_;
			return 0;
		}

		//用于在共享内存中创建时做校验
		//@return 0表示CCodeQueue对象已创建， 否则表示对象未创建
		int CheckSum() const
		{
			if(!offset_ || !size_ || !create_time_)
			{
				return -1;
			}
			unsigned int nTempInt = 0;
			nTempInt = (unsigned int)offset_ ^ (unsigned int)size_
				       ^ (unsigned int)create_time_ ^ (unsigned int) checksum_;

			return (nTempInt == 0) ? 0 : -1;
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