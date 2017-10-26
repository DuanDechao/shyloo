#include "GD.h"
#include "ILogic.h"
#include "ProtocolID.pb.h"
bool GD::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	return true;
}

bool GD::launched(sl::api::IKernel * pKernel){
	RGS_PROTO_HANDLER(SLMODULE(Logic), ClientMsgID::CLIENT_MSG_GD_COMMAND, GD::dispatchGDCommand);
	RSG_GD(SLMODULE(GD), gdTest);
	return true;
}

bool GD::destory(sl::api::IKernel * pKernel){
	for (auto itor = _gdCommands.begin(); itor != _gdCommands.end(); ++itor){
		DEL itor->second;
	}
	_gdCommands.clear();

	return true;
}

bool GD::dispatchGDCommand(sl::api::IKernel* pKernel, IObject* object, const sl::OBStream& args){
	const char* command = nullptr;
	if (!args.readString(command)){
		SLASSERT(false, "wtf");
		return false;
	}

	int32 errCode = dealCommand(pKernel, object, command);
	return true;
}

int32 GD::dealCommand(sl::api::IKernel* pKernel, IObject* object, const char* args){
	char command[512] = { 0 };
	sl::SafeMemcpy(command, sizeof(command), args, strlen(args));
	std::vector<std::string> subStrings;
	splitString(args, subStrings);
	
	char * p = strstr(command, " ");
	if (p)
		*p = 0;
	std::transform(command, command + strlen(command), command, ::tolower);

	char gdArgs[512] = { 0 };
	if (subStrings.size() > 1){
		for (int32 i = 1; i < subStrings.size(); i++){
			int32 curEndPos = strlen(gdArgs);
			sl::SafeMemcpy(gdArgs + curEndPos, sizeof(gdArgs), subStrings[i].c_str(), subStrings[i].length());
			gdArgs[strlen(gdArgs)] = ' ';
		}
		gdArgs[strlen(gdArgs) - 1] = '\0';
	}

	auto itor = _gdCommands.find(command);
	if (itor != _gdCommands.end()){
		return itor->second->dealCommand(pKernel, object, gdArgs);
	}
	else{
		ECHO_ERROR("unknown gd cmd %s", command);
	}
	return gd::GD_NONE;
}

void GD::splitString(const char* commandStr, std::vector<std::string>& cmdStrings){
	char temp[512] = { 0 };
	int32 strLen = strlen(commandStr);
	sl::SafeMemcpy(temp, sizeof(temp), commandStr, strLen);
	char* p = temp;
	for (int32 i = 0; i <= strLen; i++){
		if (temp[i] == ' ' || i == strLen){
			temp[i] = '\0';
			string subString = p;
			if (subString.length() > 0)
				cmdStrings.push_back(subString);
			p = temp + i + 1;
		}
	}
}

void GD::rsgGDInner(const char* command, IGDCommandHandler* handler){
	char tmp[256] = { 0 };
	tmp[0] = '&';
	tmp[1] = '&';
	SafeSprintf(tmp + 2, sizeof(tmp) - 2, "%s", command);
	std::transform(tmp, tmp + strlen(tmp), tmp, ::tolower);
	_gdCommands[tmp] = handler;
}

int32 GD::gdTest(sl::api::IKernel* pKernel, IObject* object, int32 type){
	int32 i = 0;
	i = 7;
	return 0;
}