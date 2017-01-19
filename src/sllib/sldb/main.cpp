#include "sldb.h"
using namespace sl::db;
int main(){
	ISLDBConnection* conn = newConnection();
	if (conn->open("127.0.0.1", 3306, "root", "", "vrgame", "utf-8")){
		const char* query = "INSERT vrgame(itemid, name) values (323234, \'dsdfsd\') ";
		ISLDBResult* res = conn->execute(query);
		res->release();
	}
	return 0;
}

