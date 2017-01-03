#cmake file
SET(SL_LOGIC_INTERFACE
	"${PROJECT_SOURCE_DIR}/src/logic/interface/IHarbor.h"
)

SET(SL_LOGIC_DEFINE
	"${PROJECT_SOURCE_DIR}/src/logic/define/NodeDefine.h"
	"${PROJECT_SOURCE_DIR}/src/logic/define/NodeProtocol.h"
) 
 
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/harbor)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/slave)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/starter)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/master)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/logic/cluster)