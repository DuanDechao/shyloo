#include "appconfig.h"
using namespace sl;

#define MAIN_SECTION	"Global"

//intÐÍ
#define GETINT(section, name, value) \
	value = cfg.GetInt(section, name, &iRet); \
	if(iRet) \
{ \
}

//CsizeStringÊý×é
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

int CAppConfig::LoadConfig()
{
	CConfig cfg;
	int iRet = cfg.LoadConfig(APP_CONF_PATH);
	if(iRet)
	{
		SL_ERROR("fail load config file %s, iRet=%d", APP_CONF_PATH, iRet);
		return iRet;
	}
	GETINT(MAIN_SECTION, "ServerID", ServerID);
	GETSTRING(MAIN_SECTION, "Platform", Platform);
	GETSTRING(MAIN_SECTION, "SoPath", SoPath);
	GETSTRING(MAIN_SECTION, "CmdFactoryConf", CmdFactoryConf);

	GETSTRING(MAIN_SECTION, "CodeStreamKey", CodeStreamKey);
	GETINT(MAIN_SECTION, "CodeStreamSize", CodeStreamSize);
	GETSTRING(MAIN_SECTION, "CodeFrontEndSocket", CodeFrontEndSocket);
	GETSTRING(MAIN_SECTION, "CodeBackEndSocket", CodeBackEndSocket);
	GETSTRING(MAIN_SECTION, "LogFormat", LogFormat);
	GETSTRING(MAIN_SECTION, "LogFilter", LogFilter);
	GETINT(MAIN_SECTION, "LogFileNum", LogFileNum);
	GETINT(MAIN_SECTION, "LogFileSize", LogFileSize);
	GETINT(MAIN_SECTION, "DBWriteBackTime", DBWriteBackTime);

	return SetLogConfig();
}