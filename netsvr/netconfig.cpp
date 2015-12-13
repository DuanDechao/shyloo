#include "netconfig.h"

using namespace sl;

#define MAIN_SECTION	"Global"

//int型
#define GETINT(section, name, value) \
	value = cfg.GetInt(section, name, &iRet); \
	if(iRet) \
	{ \
	}

//CsizeString数组
#define GETSIZESTRING(section, name, value) \
	s = cfg.GetString(section, name, &iRet); \
	if(iRet) \
	{ \
	} \
	iRet = value.Set("%s", s.c_str()); \
	if(iRet < 0) \
	{\
	}
// string
#define GETSTRING(section, name, value) \
	value = cfg.GetString(section, name, &iRet); \
	if(iRet) \
	{ \
	}
int CNetConfig::LoadConfig()
{
	CConfig cfg;
	string s;
	int iRet = cfg.LoadConfig(ConfigName.Get());
	if(iRet)
	{
	}
	GETSIZESTRING(MAIN_SECTION, "MgrShmKey", MgrShmKey);

	GETSIZESTRING(MAIN_SECTION, "EnpStreamKey", FrontEndShmKey);
	GETINT(MAIN_SECTION, "EnpStreamSize", FrontEndShmSize);
	GETSIZESTRING(MAIN_SECTION, "EnpFrontEndSocket", FrontEndSocket);
	GETSIZESTRING(MAIN_SECTION, "EnpBackEndSocket", BackEndSocket);

	GETINT(MAIN_SECTION, "RecvBufferSize", RecvBufferSize);
	GETINT(MAIN_SECTION, "SendBufferSize", SendBufferSize);
	GETINT(MAIN_SECTION, "BufferCount", BufferCount);

	GETINT(MAIN_SECTION, "MaxConnect", SocketMaxCount);
	GETINT(MAIN_SECTION, "ListenCount", ListenArray.m_iUsedCount);

	CSizeString<32> section;
	int iTemp = 0;
	for (int i = 0; i< ListenArray.m_iUsedCount; ++i)
	{
		section.Set("Listen%d", i+1);
		GETSIZESTRING(section(), "ListenIP", ListenArray[i].m_szListenIP);

		GETINT(section(), "ListenPort", iTemp);
		ListenArray[i].m_unListenPort = static_cast<unsigned int>(iTemp);

		GETINT(section(), "IdleTimeout", ListenArray[i].m_uiIdleTimeOut);

		GETSTRING(section(), "Package", s);
		CStringUtils::TrimLeft(s, "\t\r\n");
		CStringUtils::TrimRight(s, "\t\r\n");
		if(CStringUtils::CompareNoCase(s, "binary") == 0)
		{
			ListenArray[i].m_stPackageType  =  NET_PACKAGE_BINARY;
			ListenArray[i].m_ucDecodeMethod =  CODE_BIN;
		}
		else if(CStringUtils::CompareNoCase(s, "string") == 0)
		{
			ListenArray[i].m_stPackageType  =  NET_PACKAGE_STRING;
			ListenArray[i].m_ucDecodeMethod =  CODE_STRING;
		}
		else if(CStringUtils::CompareNoCase(s, "encrypted") == 0)
		{
			ListenArray[i].m_stPackageType = NET_PACKAGE_ENCRYPTED;
			ListenArray[i].m_ucDecodeMethod = CODE_ENCRYPTED;
		}
		else if(CStringUtils::CompareNoCase(s, "flash") == 0)
		{
			ListenArray[i].m_stPackageType = NET_PACKAGE_FLASH;
			ListenArray[i].m_ucDecodeMethod = CODE_STRING;
		}
		else
		{

		}
		///编码方式
		GETSTRING(section(), "EncodeType", s);
		CStringUtils::TrimLeft(s, "\t\r\n");
		CStringUtils::TrimRight(s, "\t\r\n");
		if(CStringUtils::CompareNoCase(s, "binary") == 0)
		{
			ListenArray[i].m_ucEncodeMethod = CODE_BIN;
		}
		else if(CStringUtils::CompareNoCase(s, "string") == 0)
		{
			ListenArray[i].m_ucEncodeMethod = CODE_STRING;
		}
		else if(CStringUtils::CompareNoCase(s, "json") == 0)
		{
			ListenArray[i].m_ucEncodeMethod = CODE_JSON;
		}
		else
		{

		}
	}

	///流量控制配置
	GETINT(MAIN_SECTION, "MaxConnect", MaxConnect);
	GETINT(MAIN_SECTION, "NewConnCheckInterval", NewConnCheckInterval);
	GETINT(MAIN_SECTION, "NewConnMax", NewConnMax);
	GETINT(MAIN_SECTION, "SendUpCheckInterval", SendUpCheckInterval);
	GETINT(MAIN_SECTION, "SendUpMax", SendUpMax);
	GETINT(MAIN_SECTION, "StopAcceptInterval", StopAcceptInterval);

	return LoadLogConfig();

}

int CNetConfig::ReloadConfig()
{
	return LoadLogConfig();
}

int CNetConfig::LoadLogConfig()
{
	CConfig cfg;
	int iRet = cfg.LoadConfig(ConfigName.Get());
	if(iRet)
	{

	}

	int iLogFileNum = 0, iLogFileSize = 0;
	GETINT(MAIN_SECTION, "LogFileNum", iLogFileNum);
	GETINT(MAIN_SECTION, "LogFileSize", iLogFileSize);
	SL_NLOG->Init(ENamed, NULL, FILE_NORMALLOG, iLogFileSize, iLogFileNum);
	SL_ELOG->Init(ENamed, NULL, FILE_ERRORLOG, iLogFileSize, iLogFileNum);
	//SL_NLOG->Init(ENamed, NULL, FILE_STATLOG, iLogFileSize, iLogFileNum);

	//设置format
	string str;
	GETSTRING(MAIN_SECTION, "LogFormat", str);
	SL_NLOG->SetFormatByStr(str.c_str());
	SL_ELOG->SetFormatByStr(str.c_str());

	//设置filter
	GETSTRING(MAIN_SECTION, "LogFilter", str);
	SL_NLOG->SetFilterByStr(str.c_str());
	SL_ELOG->SetFilterByStr(str.c_str());

	SL_INFO("LogFormat=%d LogFilter=%d", SL_NLOG->GetFormat(),SL_NLOG->GetFilter());
	return 0;


}