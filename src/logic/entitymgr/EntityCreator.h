#ifndef SL_LOGIC_ENTITY_CREATOR_H
#define SL_LOGIC_ENTITY_CREATOR_H
#include "slikernel.h"
#include "IEntityDef.h"
class EntityMgr : public IEntityMgr{
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

private:
	EntityMgr*				_self;
	sl::api::IKernel*		_kernel;
};
#endif