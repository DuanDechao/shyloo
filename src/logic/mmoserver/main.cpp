#include "BaseApp.h"
#include "CellApp.h"
#include "DBMgr.h"
#include "BaseAppMgr.h"
#include "CellAppMgr.h"
using namespace sl::api;

GET_DLL_ENTRANCE

CREATE_MODULE(BaseApp)
CREATE_MODULE(CellApp)
CREATE_MODULE(DBMgr)
CREATE_MODULE(BaseAppMgr)
CREATE_MODULE(CellAppMgr)
