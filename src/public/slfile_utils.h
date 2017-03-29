//读写文件的封装类

#ifndef _SL_FILE_UTILS_H_
#define _SL_FILE_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string>
#include <fcntl.h>
#include <io.h>
#include <vector>
#include <functional>
#include <Shlwapi.h>
namespace sl
{
	//读写文件的封装类
	class CFileUtils
	{
	public:
		//获取文件的长度
		/*
			@param [in]   pszFileName  要计算长度的文件
			@param [out]  lFileSize    保存的文件长度
			@return 0表示成功， 其他表示失败
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

		//把文件的数据全部读入缓冲区中
		/*
			@param [in]   pszFileName    要读取的文件
			@param [out]  ppszBuf        保存文件内容的缓存区，这个缓存区是在ReadFile内部new， 调用者需要手动delete
			@param [out]  iBufLen        保存了文件的长度
			@return 0表示成功，其他值表示失败
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
				//获取文件长度
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

				//循环读取文件数据
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

		//把文件的数据全部读入缓冲区中
		/*
			@param [in]  pszFileName  要读取的文件
			@param [out] sBuf         保存文件内容的缓冲区
			@return 0表示成功 其他值表示失败
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

		//把缓冲区的数据全部写入文件
		/*
			@param pszFileName [in]  要写入的文件
			@param pszBuf      [in]  存有数据的缓冲区
			@param iBufLen     [in]  缓冲区中数据的大小
			@param bAppend     [in]  是否追加写入
			@return 0表示成功 其他表示失败
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

			//循环写入数据
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

		//把缓冲区的数据全部写入文件
		/*
			@param [in] pszFileName  要写入的文件
			@param [in] sBuf         存有数据的缓冲区
			@param [in] bAppend      是否追加写入
			@return 0表示成功 其他表示失败
		*/
		static int WriteFile(const char* pszFileName, const std::string& sBuf, bool bAppend = false)
		{
			return WriteFile(pszFileName, sBuf.c_str(), sBuf.size(), bAppend);
		}

	}; // class CFileUtils


	void ListFileInDirection(const char * path, const char * extension, const std::function<void(const char *, const char *)> &f) {
#ifdef SL_OS_WINDOWS
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
#else
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
				if (0 == strcmp(extension, GetFileExt(dirp->d_name))) {
					char name[256];
					SafeSprintf(name, sizeof(name), "%s", dirp->d_name);
					char * dot = strrchr(name, '.');
					if (dot != nullptr)
						*dot = 0;
					f(name, tmp);
				}
			}
		}
#endif
	}
	
}// namespace sl
#endif