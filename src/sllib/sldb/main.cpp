#include "sldb.h"
#include "slmulti_sys.h"
using namespace sl::db;
int main(){
	ISLDBConnection* conn = newConnection();
	if (conn->open("127.0.0.1", 3306, "root", "", "shyloo", "utf8")){
		const char* query = "SELECT * FROM user";
		ISLDBResult* res = conn->execute(query);
		if (res){
			while (res->next()){
				ECHO_TRACE("%lld, %s", res->toInt64(0), res->toString(1));
			}
		}
	}
	return 0;
}

