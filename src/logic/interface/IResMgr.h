#ifndef _SL_INTERFACE_RESMGR_H__
#define _SL_INTERFACE_RESMGR_H__
#include "slimodule.h"
#include <string>
class IResMgr : public sl::api::IModule{
public:
	virtual ~IResMgr() {}
	virtual std::string getPySysResPath() = 0;
	virtual std::string getPyUserResPath() = 0;
	virtual std::string getPyUserScriptsPath() = 0; 
	virtual bool hasRes(const std::string& res) = 0;
	virtual std::string matchRes(const std::string& res) = 0;
};

#endif
