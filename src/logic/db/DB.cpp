#include "DB.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IMysqlMgr.h"
#include "DBCall.h"

bool DB::initialize(sl::api::IKernel * pKernel){
	_self = this;
	_kernel = pKernel;
	return true;
}

bool DB::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_mysql, MysqlMgr);

	test();
	return true;
}

bool DB::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

IDBCall* DB::create(int64 threadId, const int64 id, const char* file, const int32 line, const void* context, const int32 size){
	if (size <= SIZE_32){
		return DBContext<SIZE_32>::create(_self, threadId, id);
	}
	else if (size <= SIZE_64){
		return DBContext<SIZE_64>::create(_self, threadId, id);
	}
	else if (size <= SIZE_128){
		return DBContext<SIZE_128>::create(_self, threadId, id);
	}
	else if (size <= SIZE_256){
		return DBContext<SIZE_256>::create(_self, threadId, id);
	}
	else if (size <= SIZE_512){
		return DBContext<SIZE_512>::create(_self, threadId, id);
	}
	else if (size <= SIZE_1024){
		return DBContext<SIZE_1024>::create(_self, threadId, id);
	}
	else if (size <= SIZE_2048){
		return DBContext<SIZE_2048>::create(_self, threadId, id);
	}
	else if (size <= SIZE_4096){
		return DBContext<SIZE_4096>::create(_self, threadId, id);
	}
	else{
		SLASSERT(false, "too large");
	}
	return nullptr;
}

void DB::test(){
	IDBCall* dbcall = DBContext<SIZE_64>::create(_self, 0, 0);
}




