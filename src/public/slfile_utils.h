//��д�ļ��ķ�װ��

#ifndef _SL_FILE_UTILS_H_
#define _SL_FILE_UTILS_H_

#include "slmulti_sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string>
#include <fcntl.h>
#include <vector>
#include <functional>

#ifdef SL_OS_WINDOWS
#include <Shlwapi.h>
#include <io.h>
#endif

namespace sl
{
//��д�ļ��ķ�װ��
class CFileUtils{
public:
	//��ȡ�ļ��ĳ���
	/*
		@param [in]   pszFileName  Ҫ���㳤�ȵ��ļ�
		@param [out]  lFileSize    ������ļ�����
		@return 0��ʾ�ɹ��� ������ʾʧ��
	*/
	static int GetFileSize(const char* pszFileName, long& lFileSize){
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
	static int ReadFile(const char* pszFileName, char** ppszBuf, size_t& iBufLen){
		if(pszFileName == NULL){
			return -1;
		}
		FILE* pstFile = 0;
#ifdef SL_OS_WINDOWS
		errno_t err = fopen_s(&pstFile, pszFileName, "rb");
		if (err || !pstFile)
			return -2;
#else
		pstFile = fopen(pszFileName, "rb");
		if (pstFile == NULL){
			return -2;
		}
#endif
		int iRet = 0;
		do{
			//��ȡ�ļ�����
			iRet = fseek(pstFile, 0, SEEK_END);
			if(iRet){
				iRet = -3;
				break;
			}

			iBufLen = ftell(pstFile);
			if(iBufLen < 0){
				iRet = -4;
				break;
			}

			iRet = fseek(pstFile, 0, SEEK_SET);
			if(iRet){
				iRet = -5;
				break;
			}

			char* pszBuf = new char[iBufLen + 1];
			*ppszBuf = pszBuf;
			if(pszBuf == NULL){
				iRet = -6;
				break;
			}

			size_t iLeft = iBufLen;
			size_t iRead = 0;
			size_t nRead = 0;

			//ѭ����ȡ�ļ�����
			while(!feof(pstFile) && iRead < iBufLen){
				nRead  = fread(pszBuf + iRead, sizeof(char), iLeft, pstFile);
				iLeft -= nRead;
				iRead += nRead;

				iRet = ferror(pstFile);
				if(iRet){
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

	static int ReadFile(const char* pszFileName, std::string& sBuf){
		sBuf.clear();

		char* pszBuf = NULL;
		size_t iBufLen = 0;
		int iRet = ReadFile(pszFileName, &pszBuf, iBufLen);
		if(iRet == 0 && pszBuf != NULL && iBufLen > 0){
			sBuf.assign(pszBuf, iBufLen);
		}

		if(pszBuf){
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
	static int WriteFile(const char* pszFileName, const char* pszBuf, size_t iBufLen, bool bAppend = false){
		if(pszFileName == NULL || pszBuf == NULL || iBufLen <= 0){
			return -1;
		}

		const char* mode = bAppend ? "a+b" : "w+b";
		FILE* pstFile = 0;
#ifdef SL_OS_WINDOWS
		errno_t err = fopen_s(&pstFile, pszFileName, mode);
		if (err || !pstFile)
			return -2;
#else
		pstFile = fopen(pszFileName, mode);
		if (pstFile == NULL)
			return -2;
#endif
		size_t iLeft  = iBufLen;
		size_t iWrite = 0;
		size_t nWrite = 0;
		int iRet = 0;

		//ѭ��д������
		while(iWrite < iBufLen){
			nWrite  = fwrite(pszBuf, sizeof(char), iLeft, pstFile);
			iLeft  -= nWrite;
			iWrite += nWrite;

			iRet = ferror(pstFile);
			if(iRet){
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
	static int WriteFile(const char* pszFileName, const std::string& sBuf, bool bAppend = false){
		return WriteFile(pszFileName, sBuf.c_str(), sBuf.size(), bAppend);
	}

	//���ĳ�ļ�����ָ���ļ�·��
	/*
	@param [in] path		 �ļ�����
	@param [in] extension    �ļ���׺
	@param [in] f			 ������
	@return ��
	*/
#ifdef SL_OS_WINDOWS
	static void ListFileInDirection(const char * path, const char * extension, const std::function<void(const char *, const char *)> &f) {
		WIN32_FIND_DATA finder;

		char tmp[512] = { 0 };
		SafeSprintf(tmp, sizeof(tmp), "%s/*.*", path);

		HANDLE handle = FindFirstFileA(tmp, &finder);
		if (INVALID_HANDLE_VALUE == handle)
			return;

		while (::FindNextFileA(handle, &finder)) {
			if (strcmp(finder.cFileName, ".") == 0 || strcmp(finder.cFileName, "..") == 0)
				continue;

			SafeSprintf(tmp, sizeof(tmp), "%s/%s", path, finder.cFileName);
			if (finder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				ListFileInDirection(tmp, extension, f);
			else {
				if (0 == strcmp(extension, ::PathFindExtension(finder.cFileName))) {
					PathRemoveExtension(finder.cFileName);
					f(finder.cFileName, tmp);
				}
			}
		}
	}
#else
	static const char* get_filename_ext(const char* fileName){
		const char* dot = strrchr(fileName, '.');
		if(!dot || dot == fileName) return "";
		return dot;
	}

	static void ListFileInDirection(const char * path, const char * extension, const std::function<void(const char *, const char *)> &f) {
		DIR * dp = opendir(path);
		if (dp == nullptr)
			return;

		struct dirent * dirp;
		while ((dirp = readdir(dp)) != nullptr) {
			if (dirp->d_name[0] == '.')
				continue;

			char tmp[256] = { 0 };
			SafeSprintf(tmp, sizeof(tmp), "%s/%s", path, dirp->d_name);

			struct stat st;
			if (stat(tmp, &st) == -1)
				continue;

			if (S_ISDIR(st.st_mode))
				ListFileInDirection(tmp, extension, f);
			else {
				if (0 == strcmp(extension, get_filename_ext(dirp->d_name))) {
					char name[256];
					SafeSprintf(name, sizeof(name), "%s", dirp->d_name);
					char * dot = strrchr(name, '.');
					if (dot != nullptr)
						*dot = 0;
					f(name, tmp);
				}
			}
		}
	}
#endif

}; // class CFileUtils
	
}// namespace sl
#endif