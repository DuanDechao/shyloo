//���Code�Ķ���

#ifndef _SL_CODE_QUEUE_H_
#define _SL_CODE_QUEUE_H_
#include <string.h>
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
			����з���һ��Code
			@param [in]  pszBuffer  Code���ڴ��ַ
			@param [in]  iLen		Code���ڴ��С
			@return  0��ʾ�ɹ� -1��������  -2����������
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

			//ÿ�η���ĳ�������ܳ���0xFFFFFF  = 16M
			if(iLen < 0 || iLen > (size_ - SPACE - (int)sizeof(int)) || iLen > 0xFFFFFF)
			{
				return -1;
			}

			//newpos��ʾ���Put��write��λ��
			//wrap��ʾ�Ƿ�����
			const int startpos = write_ + sizeof(int);
			const int minpos   = (read_ < SPACE ? size_ : 0) + read_ - SPACE;
			int newpos   = startpos + iLen;
			int wrap = 0;
			if(newpos >= size_)
			{
				newpos -= size_;
				wrap = 1;
			}

			//�ж��Ƿ����
			if(write_ > minpos)
			{
				if(wrap && newpos > minpos) //��
				{
					return -2;
				}
			}else
			{
				if(wrap || newpos > minpos) //��
				{
					return -2;
				}
			}

			if(wrap)
			{
				//buf��ʼλ���Ѿ�������size_
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

			//Ԥ�Ȱ���һ��ͷ��������Ϊ0
			*(char*)(GetBuffer() + newpos)  =  0;

			//����write_ָ��
			wrap	=	write_;
			write_	=	newpos;
			newpos  =   write_;

			//�ѳ���д��ͷ��
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
			�����黺�����ϲ���һ��Code���������
			���������������Ϊ�˼���һ���ڴ濽��
			@param [in] pszBuffer
			@param [in] iLen
			@return 0��ȷ -1��������  -2��������
		*/
		int Put(const char* pszBuffer1, int iLen1, const char* pszBuffer2, int iLen2)
		{
			if(pszBuffer1 == NULL || pszBuffer2 == NULL || iLen2 <= 0 || iLen1 <= 0)
			{
				return -1;
			}

			//ÿ�η���ĳ�������ܳ���0xFFFFFF  = 16M
			int iLen = iLen1 + iLen2;
			if(iLen > (size_ - SPACE - (int)sizeof(int)) || iLen > 0xFFFFFF)
			{
				return -1;
			}

			//newpos�����Put��write_��λ��
			//wrap��ʾ�Ƿ�����
			//minpos�Ǳ���write�ļ���
			const int startpos = write_ + (int)sizeof(int);
			const int minpos   = (read_ < SPACE ? size_ : 0) + read_ - SPACE;
			int newpos = startpos + iLen;
			int wrap = 0;
			if(newpos >= size_)
			{
				newpos -= size_;
				wrap = 1;
			}

			//�ж��Ƿ����
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
				//buf��ʼλ���Ѿ�������size_
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

			//Ԥ�Ȱ���һ��ͷ��������Ϊ0
			*(char*)(GetBuffer() + newpos) = 0;

			//����write_ָ��
			wrap = write_;
			write_ = newpos;
			newpos = wrap;

			//�ѳ���д��ͷ��
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
			�Ӷ�����ȡ��һ��code
			@param	[in]  pszBuffer
			@param  [in]  bufLen			�������pszBuffer�ĳ���
			@param  [out] ilen				�������ǻ�ȡ���ݵĳ���
			@return 0��ȷ	-1��������	-2���������ݴ���
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

			//��ȡҪ��ȡ�����ݳ���
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
			�������һ��Code�ĳ���
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

			//��ȡҪ��ȡ�����ݳ���
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

		//ɾ��һ��Code
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

			//��ȡҪ��ȡ�����ݳ���
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

		//�����ڹ����ڴ��д���ʱ��У��
		//@return 0��ʾCCodeQueue�����Ѵ����� �����ʾ����δ����
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