#include "Redis.h"
#include "slxml_reader.h"
#include "slargs.h"
#define CONNECT_TIME_OUT 50000
bool Redis::initialize(sl::api::IKernel * pKernel){
	_slRedisMgr = sl::db::getSLRedisMgr();
	_kernel = pKernel;

	/*sl::XmlReader svrConf;
	if (!svrConf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "cant load core file");
		return false;
	}
	const sl::ISLXmlNode& slaves = svrConf.root()["redis"][0]["slave"];
	for (int32 i = 0; i < slaves.count(); i++){
		const char* ip = slaves[i].getAttributeString("ip");
		const int32 port = slaves[i].getAttributeInt32("port");
		const char* passwd = "";
		if (slaves[i].hasAttribute("passwd"))
			passwd = slaves[i].getAttributeString("passwd");

		sl::db::ISLRedisConnection* redisConn = _slRedisMgr->create(ip, port, passwd, CONNECT_TIME_OUT);
		if (!redisConn){
			SLASSERT(false, "create redis connection failed");
			return false;
		}
		
		Context ctx;
		ctx._conn = redisConn;
		_redisContexts.push_back(ctx);
	}

	sl::XmlReader scriptReader;
	std::string scriptPath = std::string(pKernel->getEnvirPath()) + "/redis/redis_script.xml";
	if (!scriptReader.loadXml(scriptPath.c_str())){
		SLASSERT(false, "cant load core file");
		return false;
	}
	const sl::ISLXmlNode& scripts = scriptReader.root()["script"];
	for (int32 i = 0; i < scripts.count(); i++){
		const char* proc = scripts[i].getAttributeString("name");
		const char* script = scripts[i].text();
		_scripts[proc] = script;
	}
*/
	return true;
}
bool Redis::launched(sl::api::IKernel * pKernel){
	test();
	return true;
}
bool Redis::destory(sl::api::IKernel * pKernel){
	if (_slRedisMgr)
		_slRedisMgr->release();

	DEL this;
	return true;
}

bool Redis::exec(const int64 id, const char* command, const OArgs& args, const std::function<bool(sl::api::IKernel *, const sl::db::ISLRedisResult *)>& f /* = nullptr */){
	sl::db::ISLRedisConnection* redisConn = _redisContexts[(uint64)id % (uint64)_redisContexts.size()]._conn;
	
	CommandBuf buf;
	buf._size = 0;

	SafeSprintf(buf._data + buf._size, sizeof(buf._data) - buf._size, "*%d\r\n$%d\r\n%s\r\n", args.getCount() + 1, (int32)strlen(command), command);
	buf._size += (int32)strlen(buf._data + buf._size);

	append(buf, args);

	 bool ret = redisConn->exec(buf._data, buf._size, [&](sl::db::ISLRedisResult* result){
		if (f){
			return f(_kernel, result);
		}
		return true;
	});
	return ret;
}

bool Redis::call(const int64 id, const char* proc, const int32 keyCount, const OArgs& args, const std::function<bool(sl::api::IKernel *, const sl::db::ISLRedisResult *)>& f /* = nullptr */){
	if (!loadScript(_kernel, id, proc))
		return false;

	CommandBuf buf;
	buf._size = 0;

	const char* scriptId = _redisContexts[(uint64)id % (uint64)_redisContexts.size()]._scriptIds[proc].c_str();
	int32 len = (int32)strlen(scriptId);

	char keyCountStr[32] = { 0 };
	SafeSprintf(keyCountStr, 32, "%d", keyCount);
	SafeSprintf(buf._data + buf._size, (int32)sizeof(buf._data) - buf._size, "*%d\r\n$7\r\nEVALSHA\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n", args.getCount() + 3, len, scriptId, (int32)strlen(keyCountStr), keyCountStr);
	buf._size += (int32)strlen(buf._data + buf._size);

	append(buf, args);

	Context& ctx = _redisContexts[(uint64)id % (uint64)_redisContexts.size()];
	bool ret = ctx._conn->exec(buf._data, buf._size, [&](sl::db::ISLRedisResult* result){
		if (f)
			return f(_kernel, result);
		return true;
	});
	return ret;
}

bool Redis::rankInsert(const char* map, const char* sort, const int64 id, const int64 score, const void* context, const int32 size){
	IArgs<5, 1024> args;
	args << map << sort << id;
	args.addStruct(context, size);
	args << score;
	args.fix();
	return call(0, "db_rank_insert", 3, args.out());
}

bool Redis::loadScript(sl::api::IKernel* pKernel, const int64 id, const char* proc){
	Context& ctx = _redisContexts[(uint64)id % (uint64)_redisContexts.size()];
	auto itor = _scripts.find(proc);
	if (itor != _scripts.end()){
		auto itorId = ctx._scriptIds.find(proc);
		if (itorId == ctx._scriptIds.end()){
			IArgs<2, 2048> args;
			args << "LOAD" << itor->second.c_str();
			args.fix();
			return exec(id, "SCRIPT", args.out(), [&ctx, proc](sl::api::IKernel* pKernel, const sl::db::ISLRedisResult* result){
				ctx._scriptIds[proc] = result->toString();
				return true;
			});
		}
	}
	return true;
}
void Redis::append(CommandBuf& buf, const OArgs& args){
	for (int32 i = 0; i < args.getCount(); i++){
		switch (args.getType(i)){
		case ARGS_TYPE_BOOL: append(buf, args.getBool(i) ? 1 : 0); break;
		case ARGS_TYPE_INT8: append(buf, args.getInt8(i)); break;
		case ARGS_TYPE_INT16: append(buf, args.getInt16(i)); break;
		case ARGS_TYPE_INT32: append(buf, args.getInt32(i)); break;
		case ARGS_TYPE_INT64: append(buf, args.getInt64(i)); break;
		case ARGS_TYPE_FLOAT: append(buf, args.getFloat(i)); break;
		case ARGS_TYPE_STRING: append(buf, args.getString(i)); break;
		case ARGS_TYPE_STRUCT: { int32 size = 0; const void* val = args.getStruct(i, size); append(buf, val, size); break; }
		default: SLASSERT(false, "wrtf"); break;
		}
	}
}

void Redis::append(CommandBuf& buf, int8 val){
	char temp[32] = { 0 };
	SafeSprintf(temp, sizeof(temp), "%d", val);

	SafeSprintf(buf._data + buf._size, sizeof(buf._data) - buf._size, "$%d\r\n%s\r\n", (int32)strlen(temp), temp);
	buf._size += (int32)strlen(buf._data + buf._size);
}

void Redis::append(CommandBuf& buf, int16 val){
	char temp[32] = { 0 };
	SafeSprintf(temp, sizeof(temp), "%d", val);

	SafeSprintf(buf._data + buf._size, sizeof(buf._data) - buf._size, "$%d\r\n%s\r\n", (int32)strlen(temp), temp);
	buf._size += (int32)strlen(buf._data + buf._size);
}

void Redis::append(CommandBuf& buf, int32 val){
	char temp[32] = { 0 };
	SafeSprintf(temp, sizeof(temp), "%d", val);

	SafeSprintf(buf._data + buf._size, sizeof(buf._data) - buf._size, "$%d\r\n%s\r\n", (int32)strlen(temp), temp);
	buf._size += (int32)strlen(buf._data + buf._size);
}

void Redis::append(CommandBuf& buf, int64 val){
	char temp[32] = { 0 };
	SafeSprintf(temp, sizeof(temp), "%lld", val);

	SafeSprintf(buf._data + buf._size, sizeof(buf._data) - buf._size, "$%d\r\n%s\r\n", (int32)strlen(temp), temp);
	buf._size += (int32)strlen(buf._data + buf._size);
}

void Redis::append(CommandBuf& buf, float val){
	char temp[32] = { 0 };
	SafeSprintf(temp, sizeof(temp), "%.2f", val);

	SafeSprintf(buf._data + buf._size, sizeof(buf._data) - buf._size, "$%d\r\n%s\r\n", (int32)strlen(temp), temp);
	buf._size += (int32)strlen(buf._data + buf._size);
}

void Redis::append(CommandBuf& buf, const char* val){
	SafeSprintf(buf._data + buf._size, sizeof(buf._data) - buf._size, "$%d\r\n%s\r\n", (int32)strlen(val), val);
	buf._size += (int32)strlen(buf._data + buf._size);
}

void Redis::append(CommandBuf& buf, const void* val, const int32 size){
	SafeSprintf(buf._data + buf._size, sizeof(buf._data) - buf._size, "$%d\r\n", size);
	buf._size += (int32)strlen(buf._data + buf._size);

	sl::SafeMemcpy(buf._data + buf._size, sizeof(buf._data) - buf._size, val, size);
	buf._size += size;
	buf._data[buf._size] = '\r';
	buf._data[buf._size + 1] = '\n';
	buf._size += 2;
	/*SafeSprintf(buf._data + buf._size, sizeof(buf._data) - buf._size, "\r\n");
	buf._size += (int32)strlen(buf._data + buf._size);*/
}

void Redis::test(){
	/*IArgs<2, 128> args;
	args << "foo" << 35454646465;
	args.fix();
	exec(0, "set", args.out());*/
}
