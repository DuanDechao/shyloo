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
	return true;
}

bool DB::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

IDBCall* DB::create(int64 threadId, const int64 id, const char* file, const int32 line, const void* context, const int32 size){
	if (size <= SIZE_32){
		return NEW DBContext<SIZE_32>(_self, threadId, id);
	}
	else if (size <= SIZE_64){
		return NEW DBContext<SIZE_64>(_self, threadId, id);
	}
	else if (size <= SIZE_128){
		return NEW DBContext<SIZE_128>(_self, threadId, id);
	}
	else if (size <= SIZE_256){
		return NEW DBContext<SIZE_256>(_self, threadId, id);
	}
	else{
		SLASSERT(false, "too large");
	}
	return nullptr;
}




