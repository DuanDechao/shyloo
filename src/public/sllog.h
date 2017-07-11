//shyloo的log类
/********************************************************************
	created:	2015/12/04
	created:	4:12:2015   20:52
	filename: 	e:\myproject\shyloo\sllib\sllog.h
	file path:	e:\myproject\shyloo\sllib
	file base:	sllog
	file ext:	h
	author:		ddc
	
	purpose:	shyloo的log类
*********************************************************************/

#ifndef _SL_LOG_H_
#define _SL_LOG_H_
#include "slsingleton.h"
#include "slplatform.h"
#include "slstring_utils.h"
#include "sltime.h"
#ifdef SL_OS_WINDOWS
#include <io.h>
#endif
#include <sys/stat.h>

namespace sl
{
//日志类型
typedef enum ELogTypeTag{
	ENamed	=	1,		///< 指定文件名的日志，写满后会增加新文件 test.log, test1.log
	EDaily,				///< 以天为单位的日志，20060307.log 20060307-00.log
	EMonth,				///< 以月为单位的日志，200603.log 200603-01.log
	EHour,				///< 以小时为单位的日志，20090219_00.log  20090219_00-01.log
} ELogType;

//日志级别
typedef enum ElogFilterTag{
	EFatal		=	0x1,
	EError		=	0x2,
	EWarning	=	0x4,
	EInfo		=	0x8,
	EDebug		=	0x10,
	ETrace		=	0x20,

} ELogFilter;

//日志格式
typedef enum ELogFormatTag{
	ETime		=	0x1,		///< 在行首输出时间
	EProcessId  =   0x2,		///< 在行首输出进程Id
	EThreadId	=	0x4,		///< 在行首输出线程Id
	ENewLine	=	0x8,		///< 在行末输出换行
	EType		=	0x10,		///< 在行首输出日志级别
	EStdOut		=	0x20,		///< 额外的在标准输出上输出
	EDebugOut	=	0x40,		///< 在Windows平台下用DebugOutput输出
	EFileLine	=	0x80,		///< 输出所在m_pFile和m_uiLine
	ENoFile		=	0x100,		///< 不把日志写入文件
} ELogFormat;

#define DEFAULT_SUFFIX	".log"
#define DEFAULT_LOGNAME	"my.log"

//Mutex的RTTI类
template<typename MUTEX>
class CMutexGuard{
public:
	CMutexGuard(MUTEX& st): m_st(st){
		m_st.Lock();
	}
	~CMutexGuard(){
		m_st.Unlock();
	}
private:
	CMutexGuard& operator = (const CMutexGuard& obj){
		return *this;
	}
private:
	MUTEX&	m_st;
};

///空互斥类，用于单线程单进程的场景
class CNullMutex{
public:
	CNullMutex& operator= (const CNullMutex& obj) {return *this;}

	int Lock() {return 0;}
	int Unlock() {return 0;}
};

//日志类
/*
	模板参数MUTEX是采用什么互斥机制，默认是不互斥
	MUTEX对象CLogT.m_stLock需要在外部初始化
*/
template<typename MUTEX = CNullMutex>
class CLogT{
private:
	enum{
		SL_LINE_MAX			=	8192,					///< 一次最多能输出的日志长度，多出部分会截断
		SL_DEFAULT_LOGSIZE	=	16 * 1024 * 1024,		///< 默认日志文件是16M
		SL_DEFAULT_LOGNUM	=	5,						///< 默认日志文件是5个
	};

	ELogType		m_eLogType;
	unsigned int	m_uiFilter;
	unsigned int	m_uiFormat;
	int64			m_logTick;
	bool            m_tickReset;
		
	int				m_iMaxLogSize;
	int				m_iMaxLogNum;

	char			m_szLogPath[SL_PATH_MAX];
	char			m_szLogSuffix[20];
	char			m_szLine[SL_LINE_MAX];
	
public:
	MUTEX			m_stLock;
	unsigned int	m_uiLine;
	const char*		m_pFile;

public:
	CLogT(){
		m_eLogType		=	ENamed;
		m_uiFilter		=	EFatal|EError|EWarning|EInfo|EDebug|ETrace;
		m_uiFormat		=	ETime|ENewLine|EType;
		m_iMaxLogSize	=	SL_DEFAULT_LOGSIZE;
		m_iMaxLogNum	=	SL_DEFAULT_LOGNUM;
		SafeSprintf(SL_STRSIZE(m_szLogPath), "%s", DEFAULT_LOGNAME);
		m_szLogSuffix[0] = 0;
		m_szLine[0] = 0;
		m_uiLine = 0;
		m_pFile = 0;
		m_logTick = 0;
		m_tickReset = false;
	}

	virtual ~CLogT() {}

public:
	void Init(ELogType eLogType = ENamed,
		const char* szLogPath = NULL,
		const char* szLogName = NULL,
		int iMaxLogSize = SL_DEFAULT_LOGSIZE,
		int iMaxLogNum = SL_DEFAULT_LOGNUM)
	{
		m_eLogType = eLogType;

		string str = "";
		if(szLogPath)
		{
			str  = szLogPath;
			CStringUtils::TrimLeft(str, " \t\r\n");
			CStringUtils::TrimRight(str, "\t\r\n");

			//确保路径最后是\或/
			if(str.size() > 0 && str[str.size() - 1] != '/' && str[str.size() - 1] != '\\')
			{
				size_t idx = str.find_first_of("/\\");
				if(idx != string::npos)
				{
					str += str[idx];
				}
			}
		}

		if(m_eLogType == ENamed){
			m_iMaxLogSize	=	iMaxLogSize;
			m_iMaxLogNum	=	iMaxLogNum;
			SafeSprintf(SL_STRSIZE(m_szLogPath), "%s%s",
				str.c_str(), (szLogName == NULL ? DEFAULT_LOGNAME : szLogName));
		}
		else
		{
			// m_szLogSuffix	是日志文件名的附加后缀
			// 例如每日的账单文件可能有多个，用20060307-00.log 20060307-01.log的方式区分
			// 这里的 -00 -01, 这样的后最就是m_szLogSuffix
			SafeSprintf(SL_STRSIZE(m_szLogPath), "%s", str.c_str());
			SafeSprintf(SL_STRSIZE(m_szLogSuffix), "%s", (szLogName == NULL ? DEFAULT_SUFFIX : szLogName));
		}
	}

#define DO_LOG(filter, format) \
	if(!(m_uiFilter & filter) )\
	{ \
		return 0; \
	} \
	va_list ap; \
	va_start(ap, format); \
	int iRet = VLog(filter, format, ap); \
	va_end(ap); \
	return iRet

	int Log(ELogFilter filter, const char * format, ...) { DO_LOG(filter, format); }
	int Log(const char* format, ...) { DO_LOG((ELogFilter)m_uiFilter, format); }

	//不同级别的Log
	int LogFatal(const char* format, ...) {DO_LOG(EFatal, format);}
	int LogError(const char* format, ...) {DO_LOG(EError, format);}
	int LogWarning(const char* format, ...) {DO_LOG(EWarning, format);}
	int LogInfo(const char* format, ...) {DO_LOG(EInfo, format);}
	int LogDebug(const char* format, ...) {DO_LOG(EDebug, format);}
	int LogTrace(const char* format, ...) {DO_LOG(ETrace, format);}
	int LogTraceBinary(const char* buf, int len){
		if(!(ETrace & m_uiFilter)){
			return 0;
		}
		int iRet = LogBinary(ETrace, buf, len);
		return iRet;
	}

	int LogWarnBinary(const char* buf, int len){
		if(!(ETrace & m_uiFilter)){
			return 0;
		}
		int iRet = LogBinary(EWarning, buf, len);
		return iRet;
	}

	/// Log二进制
	int LogBinary(ELogFilter filter, const char* buf, int len){
		if(!buf || !len || (!(m_uiFilter & filter)) ){
			return 0;
		}

		unsigned int uiOldFormat = GetFormat();
		RemoveFormat(ETime);
		RemoveFormat(EProcessId);
		RemoveFormat(EThreadId);
		RemoveFormat(ENewLine);
		RemoveFormat(EType);
		RemoveFormat(EFileLine);

		for (int i = 0; i < len; i++){
			if(!(i % 16)){
				if(i != 0){
					Log(filter, "\n");
				}
				Log(filter, "%04d>    ", i /16 + 1);
			}
			Log(filter, "%02X ", (unsigned char)buf[i]);
		}

		Log(filter, "\n");
		SetFormat(uiOldFormat);
			
		return 0;
	}

	inline void ResetLogTick(int64 tick) { m_logTick = tick; m_tickReset = true; }
	//真正执行Log的函数
	int VLog(ELogFilter filter, const char* szFormat, va_list ap){
#ifndef _DEBUG
		if (filter == EDebug)
			return 0;
#endif
		
		if( !(m_uiFilter & filter) || !szFormat){
			return 0;
		}

		//多线程互斥
		CMutexGuard<MUTEX> stGuard(m_stLock);
			timeval		stNowTime;
			#ifndef SL_OS_WINDOWS
			gettimeofday(&stNowTime, NULL);
			#else
			stNowTime.tv_sec = 0;
			stNowTime.tv_usec = 0;
			#endif
			CTime stNow(stNowTime.tv_sec);
			//stNowTime.tv_sec = stNowTime.tv_sec;
			
		///计算Log的文件名
		char szTimeBuf[32] = {0};
		char szFilePath[SL_PATH_MAX];
		if(m_eLogType == ENamed){
			SafeSprintf(SL_STRSIZE(szFilePath), "%s", m_szLogPath);
		}
		else{
			switch(m_eLogType){
			case EDaily: stNow.Format(SL_STRSIZE(szTimeBuf), "%Y%m%d"); break;
			case EMonth: stNow.Format(SL_STRSIZE(szTimeBuf), "%Y%m"); break;
			case EHour: stNow.Format(SL_STRSIZE(szTimeBuf), "%Y%m%d_%H"); break;
			default: stNow.Format(SL_STRSIZE(szTimeBuf), "%Y%m%d");break;
			}

			if(m_szLogPath[0] != 0)
				SafeSprintf(SL_STRSIZE(szFilePath), "%s%s%s", m_szLogPath, szTimeBuf, m_szLogSuffix);
			else
				SafeSprintf(SL_STRSIZE(szFilePath), "%s%s", szTimeBuf, m_szLogSuffix);
		}

		//准备好要输出的内容
		char* pLine = m_szLine;
		int iSize = sizeof(m_szLine) - 1;
		int iRet = 0;
		m_szLine[0] = 0;

		if(m_uiFormat & ETime){
			iRet = SafeSprintf(pLine, iSize, "[%s]", m_tickReset ? sl::getTimeStr(m_logTick).c_str() : sl::getCurrentTimeStr().c_str());
			pLine += iRet;
			iSize -= iRet;
			m_tickReset = false;
		}

		if(m_uiFormat & EProcessId){
			iRet = SafeSprintf(pLine, iSize, "[%d]", static_cast<int>(GetNowProcessId()));
			pLine += iRet;
			iSize -= iRet;
		}

		if(m_uiFormat & EThreadId){
			iRet = SafeSprintf(pLine, iSize, "[%d]", static_cast<int>(GetNowProcessId()));
			pLine += iRet;
			iSize -= iRet;
		}

		if(m_uiFormat & EType){
			iRet = SafeSprintf(pLine, iSize, "[%s]", GetFilterDesc(filter));
			pLine += iRet;
			iSize -= iRet;
		}

		iRet = SafeSprintf(pLine, iSize, szFormat, ap);
		pLine += iRet;
		iSize -= iRet;

		if(m_uiFormat & EFileLine){
			iRet = SafeSprintf(pLine, iSize, " (%s:%d)", m_pFile, m_uiLine);
			pLine += iRet;
			iSize -= iRet;
		}

		if(m_uiFormat & ENewLine){
			SafeSprintf(pLine, iSize, "\n");
			pLine += iRet;
			iSize -= iRet;
		}

		///向目标输出
		if( !(m_uiFormat & ENoFile) ){
			FILE* pstFile = nullptr;
		#ifdef SL_OS_LINUX
			mode_t iMode = umask(0);
			pstFile = fopen(szFilePath, "a+");
			umask(iMode);
		#else
			pstFile = fopen(szFilePath, "a+");
		#endif
			if (pstFile){
				fprintf(pstFile, "%s", m_szLine);
				fflush(pstFile);
				fclose(pstFile);
				if(m_eLogType == ENamed)
					ShiftFiles(szFilePath, m_iMaxLogSize, m_iMaxLogNum);
				else
					ShiftFiles(szFilePath, 0x7FFFFFFF, 0x7FFFFFFF);
			}
		}

		if(m_uiFormat & EStdOut){
			printf("%s", m_szLine);
		}

	#ifdef SL_OS_WINDOWS
		if(m_uiFormat & EDebugOut){
			OutputDebugStringA(m_szLine);
		}
	#endif

		return 0;
	}

	//回滚日志文件
	/*
		@param [in] pszFilePath		要回滚的日志文件
		@param [in] iMaxLogSize		日志文件的最大大小，超过这个值就要回滚日志文件
		@param [in] iMaxLogNum		日志文件的最大个数，超过这个值得日志文件就要丢弃
		@return 返回操作的结果，0表示成功
	*/
	int ShiftFiles(const char* pszFilePath, int iMaxLogSize, int iMaxLogNum){
		struct stat stStat;
		if(stat(pszFilePath, &stStat) < 0){
			return -3; ///获取日志文件信息失败
		}
		if(stStat.st_size < iMaxLogSize){
			return 0;
		}

		char szTempBuf[SL_PATH_MAX];
		char szNewFile[SL_PATH_MAX];

		SafeSprintf(SL_STRSIZE(szTempBuf), "%s.%d", pszFilePath, iMaxLogNum - 1);
		if(_access(szTempBuf, 0) == 0){
			if(remove(szTempBuf) < 0){
				return -4;		///< 删除最后一个日志文件失败
			}
		}

		for(int i = iMaxLogNum - 2; i >= 0; --i){
			if(i == 0){
				SafeSprintf(SL_STRSIZE(szTempBuf), "%s", pszFilePath);
			}
			else{
				SafeSprintf(SL_STRSIZE(szTempBuf), "%s.%d", pszFilePath, i);
			}

			if(_access(szTempBuf, 0) == 0){
				SafeSprintf(SL_STRSIZE(szNewFile), "%s.%d", pszFilePath, i + 1);
				if(rename(szTempBuf, szNewFile) < 0){
					return -5;  ///< 重命名日志文件失败
				}
			}
		}

		return 0;
	}


	static const char* GetFilterDesc(unsigned int iFilter){
		// 两种方式获取, 这里选择第一种
	#if 1
		switch (iFilter){
		case ETrace: return "trace";
		case EDebug: return "debug";
		case EWarning: return "warn";
		case EInfo: return "info";
		case EError: return "error";
		case EFatal: return "fatal";
		default: return "";
		}

	#else
		static int aiDesc[] ={
			0,  1,  2,  0,  3,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,
			5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
			6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
		};

		static const char aszDesc[][8] ={
			"",
			"fatal",
			"error",
			"warn",
			"info",
			"debug",
			"trace"
		};

		return aszDesc[aiDesc[iFilter]];
	#endif
	}

	///消息等级相关函数
	void SetFilter(unsigned int uiFilter) {m_uiFilter = uiFilter;}
	unsigned int GetFilter() const {return m_uiFilter;}

	void AddFilter(ELogFilter filter) {m_uiFilter |= filter;}
	void RemoveFilter(ELogFilter filter) {m_uiFilter &= ~filter;}

	void SetFilterByStr(const char* szFilter){
		if(!szFilter)
			return;

		m_uiFilter = 0;

		string str = "|" + string(szFilter) + "|";
		CStringUtils::MakeLower(str);
		CStringUtils::RemoveNot(str, "abcdefghijklmnopqrstuvwxyz|");

		if(str.find("|fatal|") != string::npos) {AddFilter(EFatal);}
		if(str.find("|error|") != string::npos) {AddFilter(EError);}
		if(str.find("|warning|")!= string::npos) {AddFilter(EWarning);}
		if(str.find("|info|")!= string::npos) {AddFilter(EInfo);}
		if(str.find("|debug|") != string::npos) {AddFilter(EDebug);}
		if(str.find("|trace|") != string::npos) {AddFilter(ETrace);}

	}

	///消息格式相关函数
	void SetFormat(unsigned int uiFormat) {m_uiFormat = uiFormat;}
	unsigned int GetFormat() const {return m_uiFormat;}
	void AddFormat(ELogFormat format){m_uiFormat |= format;}
	void RemoveFormat(ELogFormat format){m_uiFormat &= ~format;}
		
	void SetFormatByStr(const char* szFormat){
		if(!szFormat)
			return;

		m_uiFormat = 0;

		string str = "|" + string(szFormat) + "|";
		CStringUtils::MakeLower(str);
		CStringUtils::RemoveNot(str,"abcdefghijklmnopqrstuvwxyz|");

		if(str.find("|time|") != string::npos) {AddFormat(ETime);}
		if(str.find("|pid|") != string::npos) {AddFormat(EProcessId);}
		if(str.find("|tid|") != string::npos) {AddFormat(EThreadId);}
		if(str.find("|newline|") != string::npos) {AddFormat(ENewLine);}
		if(str.find("|type|") != string::npos) {AddFormat(EType);}
		if(str.find("|stdout|") != string::npos) {AddFormat(EStdOut);}
		if(str.find("|debugout|") != string::npos) {AddFormat(EDebugOut);}
		if(str.find("|fileline|") != string::npos) {AddFormat(EFileLine);}
		if(str.find("|nofile|") != string::npos) {AddFormat(ENoFile);}
	}

};

typedef CLogT<CNullMutex> CLog;
	
//class CNLog : public CLog, public CSingleton<CNLog>{};
//class CELog : public CLog, public CSingleton<CELog>{};
//class CSLog : public CLog, public CSingleton<CSLog>{};

	
//SL_SINGLETON_INIT(CNLog);
//SL_SINGLETON_INIT(CELog);
//SL_SINGLETON_INIT(CSLog);

}  // namespace sl

//全局日志
#define SL_NLOG			(CNLog::getSingletonPtr())
#define SL_ELOG			(CELog::getSingletonPtr())
#define SL_STAT			(CSLog::getSingletonPtr())

#define SL_LOG(...) do {  CLog* pLog = SL_NLOG; \
    if(pLog->GetFormat() & EFileLine) { \
	pLog->m_pFile = __FILE__; \
	pLog->m_uiLine = __LINE__; } \
    pLog->Log(__VA_ARGS__);} while(0)

#define SL_WARNING(...) do {  CLog* pLog = SL_NLOG; \
	if(pLog->GetFormat() & EFileLine) { \
	pLog->m_pFile = __FILE__; \
	pLog->m_uiLine = __LINE__; } \
	pLog->LogWarning(__VA_ARGS__);} while(0)

#define SL_INFO(...) do {  CLog* pLog = SL_NLOG; \
	if(pLog->GetFormat() & EFileLine) { \
	pLog->m_pFile = __FILE__; \
	pLog->m_uiLine = __LINE__; } \
	pLog->LogInfo(__VA_ARGS__);} while(0)

#define SL_DEBUG(...) do {  CLog* pLog = SL_NLOG; \
	if(pLog->GetFormat() & EFileLine) { \
	pLog->m_pFile = __FILE__; \
	pLog->m_uiLine = __LINE__; } \
	pLog->LogDebug(__VA_ARGS__);} while(0)

#define SL_TRACE(...) do {  CLog* pLog = SL_NLOG; \
	if(pLog->GetFormat() & EFileLine) { \
	pLog->m_pFile = __FILE__; \
	pLog->m_uiLine = __LINE__; } \
	pLog->LogTrace(__VA_ARGS__);} while(0)

#define SL_TRACEBINARY(...) do {  CLog* pLog = SL_NLOG; \
	if(pLog->GetFormat() & EFileLine) { \
	pLog->m_pFile = __FILE__; \
	pLog->m_uiLine = __LINE__; } \
	pLog->LogTraceBinary(__VA_ARGS__);} while(0)

#define SL_WARNBINARY(...) do {  CLog* pLog = SL_NLOG; \
	if(pLog->GetFormat() & EFileLine) { \
	pLog->m_pFile = __FILE__; \
	pLog->m_uiLine = __LINE__; } \
	pLog->LogWarnBinary(__VA_ARGS__);} while(0)

#define SL_FATAL(...) do {  CLog* pLog = SL_ELOG; \
	if(pLog->GetFormat() & EFileLine) { \
	pLog->m_pFile = __FILE__; \
	pLog->m_uiLine = __LINE__; } \
	pLog->LogFatal(__VA_ARGS__); \
	pLog = SL_NLOG; \
	if(pLog->GetFormat() & EFileLine) { \
	pLog->m_pFile = __FILE__; \
	pLog->m_uiLine = __LINE__; } \
	pLog->LogFatal(__VA_ARGS__); }while(0)

#define SL_ERROR(...) do {  CLog* pLog = SL_ELOG; \
	if(pLog->GetFormat() & EFileLine) { \
	pLog->m_pFile = __FILE__; \
	pLog->m_uiLine = __LINE__; } \
	pLog->LogError(__VA_ARGS__); \
	pLog = SL_NLOG; \
	if(pLog->GetFormat() & EFileLine) { \
	pLog->m_pFile = __FILE__; \
	pLog->m_uiLine = __LINE__; } \
	pLog->LogError(__VA_ARGS__); }while(0)

#endif
