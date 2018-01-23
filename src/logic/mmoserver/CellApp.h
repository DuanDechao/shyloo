#ifndef __SL_FRAMEWORK_CELLAPP_H__
#define __SL_FRAMEWORK_CELLAPP_H__
#include "slsingleton.h"
#include "IMmoServer.h"
#include "slbinary_stream.h"
#include "slstring.h"
#include "GameDefine.h"

class CellApp : public ICellApp{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);
    
    void onCreateCellEntityFromBase(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args);
    virtual bool createEntity(const char* entityName, const uint64 entityId = 0);

private:
	sl::api::IKernel*   _kernel;
	CellApp*		    _self;
};
#endif
