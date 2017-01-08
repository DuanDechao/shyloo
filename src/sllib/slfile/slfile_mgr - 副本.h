//文件类 用于文件的读取和写入

#ifndef _SL_FILE_MGR_H_
#define _SL_FILE_MGR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "slfile.h"
namespace sl
{
namespace file
{
class CSLFile :public ISLFile{
public:
	virtual bool SLAPI close(void);
	virtual bool SLAPI flush(void);
	virtual const SLAPI char* getFileName() const;

	virtual const SLAPI char* readLine();
	virtual bool SLAPI writeLine(const char* pszBuf, int32 iLen);
};
}
}

namespace sl
{

	//文件基类 获取文件当前指针
	class CFile
	{
	public:
		CFile(): m_pszFile(NULL){}

		virtual ~CFile()
		{
			if(m_pszFile)
				fclose(m_pszFile);
			m_pszFile = NULL;
		}

		bool IsOpened()
		{
			return m_pszFile != NULL;
		}

		int Pos()
		{
			if (m_pszFile)
				return ftell(m_pszFile);
			return 0;
		}

		int Seek(int Pos)
		{
			if (m_pszFile)
			{
				return fseek(m_pszFile, Pos, SEEK_SET);
			}
			return -1;
		}

		void CloseFile()
		{
			if(m_pszFile != NULL)
			{
				fclose(m_pszFile);
				m_pszFile = NULL;
			}
		}

	protected:
		FILE* m_pszFile; // 文件对象指针

	}; // class CFile

	
	//写文件操作
	class CReadFile: public CFile
	{
	public:
		CReadFile()
		{
			m_iReadSize = 0;
			m_iBufLen = 1;
			m_pszBuf = (char*)malloc(m_iBufLen);
		}

		virtual ~CReadFile()
		{
			if(m_pszBuf)
				free(m_pszBuf);
			m_pszBuf = NULL;
		}

		//打开文件
		int Open(const char* pszFile)
		{
			if(m_pszFile)
			{
				fclose(m_pszFile);
			}
			m_pszFile = fopen(pszFile, "r");
			if(m_pszFile == NULL)
				return -1;
			return 0;
		}

		//读取一行， 以delim结尾
		int Read(char delim)
		{
			if(m_pszFile == NULL)
				return -1;
		
			int iNowPos = Pos();  //记录当前位置

			//内存不够会realloc，返回真实m_iBufLen
			m_iReadSize = (int)getdelim(&m_pszBuf, &m_iBufLen, delim, m_pszFile);

			if(m_iReadSize < 0)
			{
				m_iReadSize = 0;
				return -2;
			}

			if(m_pszBuf[m_iReadSize-1] != (char)delim) //一行没读完文件结束了
			{
				Seek(iNowPos);
				return -3;
			}
			return 0;
		}

		char* GetBuf()
		{
			return m_pszBuf;
		}

		int ReadSize()
		{
			return m_iReadSize;
		}

		//读取一行，以\n结尾
		char* ReadLine()
		{
			int Ret = Read('\n');
			if(Ret != 0)
			{
				return NULL;
			}

			char* sTemp = GetBuf();
			if(sTemp[m_iReadSize-1] != '\n')
			{
				return NULL;
			}
			sTemp[m_iReadSize-1] = '\0';
			return sTemp;
		}

	private:
		size_t  m_iBufLen;
		char*   m_pszBuf;
		int m_iReadSize;
		
	};// class CReadFile

	//写文件操作
	class CWriteFile: public CFile
	{
	public:
		CWriteFile(){}
		virtual ~CWriteFile(){}

		//写方式打开文件， 判断是否追加
		int Open(const char* pszFile, bool bAppend = false)
		{
			if(m_pszFile)
			{
				fclose(m_pszFile);
			}
			if(bAppend)
			{
				m_pszFile = fopen(pszFile, "a");
			}else
			{
				m_pszFile = fopen(pszFile, "w");
			}

			if(m_pszFile == NULL)
			{
				return -1;
			}
			return 0;
		}


		//以delim为结束符写入文件
		int Write(const char* pszBuf, int iLen, char delim)
		{
			if(pszBuf == NULL || iLen <= 0 || m_pszFile == NULL)
			{
				return -1;
			}

			int iWriteSize = fwrite(pszBuf, sizeof(char), iLen, m_pszFile);
			if(iWriteSize != iLen)
			{
				return -2;
			}

			iWriteSize = fwrite(&delim, sizeof(char), 1, m_pszFile);
			if(iWriteSize != 1)
			{
				return -3;
			}
			fflush(m_pszFile);
			return 0;
		}

		//写入一行
		int WriteLine(const char* pszline)
		{
			return Write(pszline, sizeof(pszline), '\n');
		}

	};// class CWriteFile

} // namespace sl
#endif