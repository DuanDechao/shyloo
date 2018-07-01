#ifndef __SL_LOGIC_GD_H__
#define __SL_LOGIC_GD_H__
#include "IGD.h"
#include <unordered_map>
#include "slbinary_stream.h"
class GD : public IGD{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	bool dispatchGDCommand(sl::api::IKernel* pKernel, IObject* object, const sl::OBStream& args);

	void test();

	static int32 gdTest(sl::api::IKernel* pKernel, IObject* object, int32 type);

protected:
	virtual void rsgGDInner(const char* command, IGDCommandHandler* handler);
	
private:
	int32 dealCommand(sl::api::IKernel* pKernel, IObject* object, const char* args);
	void splitString(const char* commandStr, std::vector<std::string>& cmdStrings);

private:
	GD*					_self;
	sl::api::IKernel*	_kernel;
	std::unordered_map<std::string, IGDCommandHandler*> _gdCommands;
};
#endif