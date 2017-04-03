#define SL_DLL_EXPORT
#include "slredis_mgr.h"
#include "slredis_connection.h"
namespace sl{
namespace db{
extern "C" SL_DLL_API ISLRedisMgr* SLAPI getSLRedisMgr(){
	return NEW SLRedisMgr();
}
}
}
