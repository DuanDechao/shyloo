//��д�ļ��ķ�װ��

#ifndef _SL_FILE_UTILS_H_
#define _SL_FILE_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string>
#include <fcntl.h>

namespace sl
{
	//��д�ļ��ķ�װ��
	class CFileUtils
	{
	public:
		//��ȡ�ļ��ĳ���
		/*
			@param [in]   pszFileName  Ҫ���㳤�ȵ��ļ�
			@param [out]  lFileSize    ������ļ�����
			@return 0��ʾ�ɹ��� ������ʾʧ��
		*/
		static int GetFileSize(const char* pszFileName, long& lFileSize)
		{
			lFileSize = 0;

			struct stat stStat;
			int iRet = stat(pszFileName, &stStat);
			if(iRet)
			{
				return iRet;
			}
			lFileSize = stStat.st_size;
			return 0;
		}

		//���ļ�������ȫ�����뻺������
		/*
			@param [in]   pszFileName    Ҫ��ȡ���ļ�
			@param [out]  ppszBuf        �����ļ����ݵĻ��������������������ReadFile�ڲ�new�� ��������Ҫ�ֶ�delete
			@param [out]  iBufLen        �������ļ��ĳ���
			@return 0��ʾ�ɹ�������ֵ��ʾʧ��
		*/
		static int ReadFile(const char* pszFileName, char** ppszBuf, size_t& iBufLen)
		{
			if(pszFileName == NULL)
			{
				return -1;
			}
			
			FILE* pstFile = fopen(pszFileName, "rb");
			if(pstFile == NULL)
			{
				return -2;
			}

			int iRet = 0;
			do{
				//��ȡ�ļ�����
				iRet = fseek(pstFile, 0, SEEK_END);
				if(iRet)
				{
					iRet = -3;
					break;
				}

				iBufLen = ftell(pstFile);
				if(iBufLen < 0)
				{
					iRet = -4;
					break;
				}

				iRet = fseek(pstFile, 0, SEEK_SET);
				if(iRet)
				{
					iRet = -5;
					break;
				}

				char* pszBuf = new char[iBufLen + 1];
				*ppszBuf = pszBuf;
				if(pszBuf == NULL)
				{
					iRet = -6;
					break;
				}

				size_t iLeft = iBufLen;
				size_t iRead = 0;
				size_t nRead = 0;

				//ѭ����ȡ�ļ�����
				while(!feof(pstFile) && iRead < iBufLen)
				{
					nRead  = fread(pszBuf + iRead, sizeof(char), iLeft, pstFile);
					iLeft -= nRead;
					iRead += nRead;

					iRet = ferror(pstFile);
					if(iRet)
					{
						break;
					}
				}
				iBufLen = iRead;
				pszBuf[iRead] = '\0';

			}while(0);

			fclose(pstFile);
			return iRet;
		}

		//���ļ�������ȫ�����뻺������
		/*
			@param [in]  pszFileName  Ҫ��ȡ���ļ�
			@param [out] sBuf         �����ļ����ݵĻ�����
			@return 0��ʾ�ɹ� ����ֵ��ʾʧ��
		*/

		static int ReadFile(const char* pszFileName, std::string& sBuf)
		{
			sBuf.clear();

			char* pszBuf = NULL;
			size_t iBufLen = 0;
			int iRet = ReadFile(pszFileName, &pszBuf, iBufLen);
			if(iRet == 0 && pszBuf != NULL && iBufLen > 0)
			{
				sBuf.assign(pszBuf, iBufLen);
			}

			if(pszBuf)
			{
				delete [] pszBuf;
			}
			return iRet;
		}

		//�ѻ�����������ȫ��д���ļ�
		/*
			@param pszFileName [in]  Ҫд����ļ�
			@param pszBuf      [in]  �������ݵĻ�����
			@param iBufLen     [in]  �����������ݵĴ�С
			@param bAppend     [in]  �Ƿ�׷��д��
			@return 0��ʾ�ɹ� ������ʾʧ��
		*/
		static int WriteFile(const char* pszFileName, const char* pszBuf, size_t iBufLen, bool bAppend = false)
		{
			if(pszFileName == NULL || pszBuf == NULL || iBufLen <= 0)
			{
				return -1;
			}

			FILE* pstFile = NULL;
			if(bAppend)
			{
				pstFile = fopen(pszFileName, "a+b");
			}
			else
			{
				pstFile = fopen(pszFileName, "w+b");
			}
			if(pstFile == NULL)
			{
				return -2;
			}

			size_t iLeft  = iBufLen;
			size_t iWrite = 0;
			size_t nWrite = 0;
			int iRet = 0;

			//ѭ��д������
			while(iWrite < iBufLen)
			{
				nWrite  = fwrite(pszBuf, sizeof(char), iLeft, pstFile);
				iLeft  -= nWrite;
				iWrite += nWrite;

				iRet = ferror(pstFile);
				if(iRet)
				{
					break;
				}
			}

			fflush(pstFile);
			fclose(pstFile);

			return iRet;

		}

		//�ѻ�����������ȫ��д���ļ�
		/*
			@param [in] pszFileName  Ҫд����ļ�
			@param [in] sBuf         �������ݵĻ�����
			@param [in] bAppend      �Ƿ�׷��д��
			@return 0��ʾ�ɹ� ������ʾʧ��
		*/
		static int WriteFile(const char* pszFileName, const std::string& sBuf, bool bAppend = false)
		{
			return WriteFile(pszFileName, sBuf.c_str(), sBuf.size(), bAppend);
		}

	}; // class CFileUtils

}// namespace sl
#endif