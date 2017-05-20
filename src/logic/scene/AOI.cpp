#include "AOI.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IDCCenter.h"
#include "Attr.h"

bool AOI::initialize(sl::api::IKernel * pKernel){
	_self = this;
	return true;
}

bool AOI::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	if (_harbor->getNodeType() != NodeType::LOGIC)
		return true;

	FIND_MODULE(_objectMgr, ObjectMgr);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENE_MSG_ADD_INTERESTER, AOI::onSceneAddInterester);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENE_MSG_ADD_WATCHER, AOI::onSceneAddWatcher);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENE_MSG_REMOVE_INTERESTER, AOI::onSceneRemoveInterester);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::SCENE_MSG_REMOVE_WATCHER, AOI::onSceneRemoveWatcher);

	return true;
}

bool AOI::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void AOI::onSceneAddInterester(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	int64 id = args.getInt64(0);
	int64 interesterId = args.getInt64(1);

	IObject* object = _objectMgr->findObject(id);
	SLASSERT(object, "wtf");

	ITableControl* interesters = object->findTable(OCTableMacro::AOI_INTERESTERS::TABLE_NAME);
	SLASSERT(interesters && !interesters->findRow(id), "wtf");
	interesters->addRowKeyInt64(interesterId);
}

void AOI::onSceneAddWatcher(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	int64 id = args.getInt64(0);
	int64 watcherId = args.getInt64(1);

	IObject* object = _objectMgr->findObject(id);
	SLASSERT(object, "wtf");

	ITableControl* watchers = object->findTable(OCTableMacro::AOI_WATCHERS::TABLE_NAME);
	SLASSERT(watchers && !watchers->findRow(id), "wtf");
	watchers->addRowKeyInt64(watcherId);
}

void AOI::onSceneRemoveInterester(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	int64 id = args.getInt64(0);
	int64 interesterId = args.getInt64(1);

	IObject* object = _objectMgr->findObject(id);
	SLASSERT(object, "wtf");

	ITableControl* interesters = object->findTable(OCTableMacro::AOI_INTERESTERS::TABLE_NAME);
	SLASSERT(interesters, "wtf");
	const IRow* row = interesters->findRow(id);
	SLASSERT(row, "wtf");
	DEL_TABLE_ROW(interesters, row);
}

void AOI::onSceneRemoveWatcher(sl::api::IKernel* pKernel, int32 nodeType, int32 nodeId, const OArgs& args){
	int64 id = args.getInt64(0);
	int64 watcherId = args.getInt64(1);

	IObject* object = _objectMgr->findObject(id);
	SLASSERT(object, "wtf");

	ITableControl* watchers = object->findTable(OCTableMacro::AOI_WATCHERS::TABLE_NAME);
	SLASSERT(watchers && !watchers->findRow(id), "wtf");
	const IRow* row = watchers->findRow(id);
	SLASSERT(row, "wtf");
	DEL_TABLE_ROW(watchers, row);
}