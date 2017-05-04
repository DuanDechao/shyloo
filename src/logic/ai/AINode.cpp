#include "AINode.h"
#include "IDCCenter.h"
#include "AICondition.h"

AINode::AINode(AICondition* condition)
	:_condition(condition)
{}

AINode::~AINode(){}

bool AINode::enter(sl::api::IKernel* pKernel, IObject* object, int32 level /* = 0 */){
	addLevel(object, level);

	if (!checkCondition(pKernel, object))
		return false;

	if (!onEnter(pKernel, object, level))
		return false;

	setState(object, level, AIState::AINODE_RUNNING);

	return true;
}

AINode::AIState AINode::tick(sl::api::IKernel* pKernel, IObject* object, int32 level /* = 0 */){
	if (!checkCondition(pKernel, object))
		return AIState::AINODE_FAILTURE;

	return onTick(pKernel, object, level);
}

void AINode::leave(sl::api::IKernel* pKernel, IObject* object, int32 level /* = 0 */){
	onLeave(pKernel, object, level);
	removeLevel(object, level);
}

bool AINode::checkCondition(sl::api::IKernel* pKernel, IObject* object){
	if (_condition)
		return _condition->check(pKernel, object);

	return true;
}

AINode::AIState AINode::getState(IObject* object, int32 level) const{
	ITableControl* aiTable = object->findTable(OCTableMacro::AITABLE::TABLE_NAME);
	SLASSERT(aiTable, "wtf");
	if (level < aiTable->rowCount()){
		const IRow* row = aiTable->getRow(level);
		SLASSERT(row, "wtf");
		return (AINode::AIState)row->getDataInt8(OCTableMacro::AITABLE::STATE);
	}
	return AINode::AIState::AINODE_COMPLETE;
}

void AINode::setState(IObject* object, int32 level, int8 state)const{
	ITableControl* aiTable = object->findTable(OCTableMacro::AITABLE::TABLE_NAME);
	SLASSERT(aiTable && level < aiTable->rowCount(), "find no ai table or table level is invailed for object %lld", object->getID());
	const IRow* row = aiTable->getRow(level);
	
	row->setDataInt8(OCTableMacro::AITABLE::STATE, state);
}

void AINode::setParam(IObject* object, int32 level, int32 index, int64 param) const{
	ITableControl* aiTable = object->findTable(OCTableMacro::AITABLE::TABLE_NAME);
	SLASSERT(aiTable && level < aiTable->rowCount(), "find no ai table or table level is invailed for object %lld", object->getID());

	const IRow* row = aiTable->getRow(level);
	row->setDataInt64(OCTableMacro::AITABLE::PARAM1 + index, param);
}

int64 AINode::getParam(IObject* object, int32 level, int32 index) const{
	ITableControl* aiTable = object->findTable(OCTableMacro::AITABLE::TABLE_NAME);
	SLASSERT(aiTable && level < aiTable->rowCount(), "find no ai table or table level is invailed for object %lld", object->getID());

	const IRow* row = aiTable->getRow(level);
	return row->getDataInt64(OCTableMacro::AITABLE::PARAM1 + index);
}

void AINode::addLevel(IObject* object, int32 level) const{
	ITableControl* aiTable = object->findTable(OCTableMacro::AITABLE::TABLE_NAME);
	SLASSERT(aiTable && aiTable->rowCount() == level, "find no ai table or table level is invaild for object %lld", object->getID());

	aiTable->addRow();
}

void AINode::removeLevel(IObject* object, int32 level) const{
	ITableControl* aiTable = object->findTable(OCTableMacro::AITABLE::TABLE_NAME);
	SLASSERT(aiTable && (level+1) == aiTable->rowCount(), "find no ai table or table level is invaild for object %lld", object->getID());

	aiTable->delRow(aiTable->rowCount() - 1);
}

