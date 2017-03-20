#cmake file
SET(SL_LOGIC_INTERFACE
	"${PROJECT_SOURCE_DIR}/src/logic/interface/IGate.h"
	"${PROJECT_SOURCE_DIR}/src/logic/interface/IHarbor.h"
	"${PROJECT_SOURCE_DIR}/src/logic/interface/IDCCenter.h"
	"${PROJECT_SOURCE_DIR}/src/logic/interface/IIdMgr.h"
	"${PROJECT_SOURCE_DIR}/src/logic/interface/IAgent.h"
	"${PROJECT_SOURCE_DIR}/src/logic/interface/IDB.h"
	"${PROJECT_SOURCE_DIR}/src/logic/interface/IEventEngine.h"
	"${PROJECT_SOURCE_DIR}/src/logic/interface/IFSM.h"
	"${PROJECT_SOURCE_DIR}/src/logic/interface/IMysqlMgr.h"
	"${PROJECT_SOURCE_DIR}/src/logic/interface/ICapacity.h"	
)

SET(SL_LOGIC_DEFINE
	"${PROJECT_SOURCE_DIR}/src/logic/define/NodeDefine.h"
	"${PROJECT_SOURCE_DIR}/src/logic/define/NodeProtocol.h"
	"${PROJECT_SOURCE_DIR}/src/logic/define/AgentProtocol.h"
	"${PROJECT_SOURCE_DIR}/src/logic/define/GameDefine.h"
	"${PROJECT_SOURCE_DIR}/src/logic/define/Attr.h"
) 
 
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/harbor)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/agent)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/slave)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/starter)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/capacity)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/master)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/cluster)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/dccenter)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/idmgr)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/gate)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/db)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/eventengine)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/mysqlmgr)
#ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/fsm)