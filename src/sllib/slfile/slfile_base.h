#ifndef SL_FILE_BASE_H
#define SL_FILE_BASE_H
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
namespace sl
{
namespace file
{
//文件基类 获取文件当前指针
class CFile
{
public:
	CFile() : m_pszFile(NULL){}
	
	virtual ~CFile(){
		if (m_pszFile)
			fclose(m_pszFile);
		m_pszFile = NULL;
	}

	//打开文件
	bool open(const char* pszFile, const char* mode){
		if (m_pszFile){
			fclose(m_pszFile);
		}
		m_pszFile = fopen(pszFile, mode);
		if (m_pszFile == NULL)
			return false;
		return true;
	}

	bool isOpened(){
		return m_pszFile != NULL;
	}

	int pos(){
		if (m_pszFile)
			return ftell(m_pszFile);
		return 0;
	}

	int seek(int Pos){
		if (m_pszFile){
			return fseek(m_pszFile, Pos, SEEK_SET);
		}
		return -1;
	}

	void close(){
		if (m_pszFile != NULL){
			fclose(m_pszFile);
			m_pszFile = NULL;
		}
	}

protected:
	FILE* m_pszFile; // 文件对象指针

}; // class CFile

//写文件操作
class CReadFile : public CFile
{
public:
	CReadFile(){
		m_iReadSize = 0;
		m_iBufLen = 1;
		m_pszBuf = (char*)malloc(m_iBufLen);
	}

	virtual ~CReadFile(){
		if (m_pszBuf)
			free(m_pszBuf);
		m_pszBuf = NULL;
	}

	//读取一行， 以delim结尾
	const char* read(char delim){
		if (m_pszFile == NULL)
			return NULL;

		int iNowPos = pos();  //记录当前位置

		//内存不够会realloc，返回真实m_iBufLen
		m_iReadSize = getline(&m_pszBuf, &m_iBufLen, delim, m_pszFile);
		if (m_iReadSize < 0){
			m_iReadSize = 0;
			return NULL;
		}

		//一行没读完文件结束了
		if (m_pszBuf[m_iReadSize - 1] != (char)delim){
			seek(iNowPos);
			return NULL;
		}
		return getReadBuf();
	}

	char* getReadBuf(){
		return m_pszBuf;
	}

	int getReadSize(){
		return m_iReadSize;
	}

	//读取一行，以\n结尾
	const char* readLine(){
		const char* pBuf = read('\n');
		if (pBuf == NULL){
			return NULL;
		}

		char* sTemp = getReadBuf();
		if (sTemp[m_iReadSize - 1] != '\n'){
			return NULL;
		}
		sTemp[m_iReadSize - 1] = '\0';
		return sTemp;
	}
private:
	size_t  m_iBufLen;
	char*   m_pszBuf;
	int m_iReadSize;

};// class CReadFile

//写文件操作
class CWriteFile : public CFile
{
public:
	CWriteFile(){}
	virtual ~CWriteFile(){}

	//以delim为结束符写入文件
	bool write(const char* pszBuf, int iLen, char delim){
		if (pszBuf == NULL || iLen <= 0 || m_pszFile == NULL){
			return false;
		}

		int iWriteSize = fwrite(pszBuf, sizeof(char), iLen, m_pszFile);
		if (iWriteSize != iLen){
			return false;
		}

		iWriteSize = fwrite(&delim, sizeof(char), 1, m_pszFile);
		if (iWriteSize != 1){
			return false;
		}
		fflush(m_pszFile);
		return true;
	}

	//写入一行
	bool writeLine(const char* pszline){
		return write(pszline, sizeof(pszline), '\n');
	}

};// class CWriteFile

}
}
#endif