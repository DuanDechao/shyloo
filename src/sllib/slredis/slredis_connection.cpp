#include "slredis_connection.h"
#include "slredis_callresult.h"
#include "slredis_mgr.h"
namespace sl{
namespace db{
SLRedisConnection::SLRedisConnection(SLRedisMgr* redisMgr, const char* ip, const int32 port, const char* passwd, const int32 timeOut)
	:_redisMgr(redisMgr),
	_port(port),
	_ctx(nullptr),
	_timeOut(timeOut),
	_passwd(passwd),
    _ip(ip)
{}

SLRedisConnection::~SLRedisConnection(){
	if (_ctx){
		redisFree(_ctx);
		_ctx = nullptr;
	}
}

void SLRedisConnection::release(){
	SLASSERT(_redisMgr, "wtf");
	_redisMgr->recover(this);
}

bool SLRedisConnection::reconnect(){
	if (_ctx){
		redisFree(_ctx);
		_ctx = nullptr;
	}
	
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = _timeOut;
	_ctx = redisConnectWithTimeout(_ip.c_str(), _port, tv);
	SLASSERT(_ctx, "wtf");
	if (!_ctx || _ctx->err){
		SLASSERT(false, "connect redis [%s:%d] failed", _ip.c_str(), _port);
		if (_ctx)
			redisFree(_ctx);
		return false;
	}

	if (_passwd != ""){
		redisReply *reply = static_cast<redisReply *>(redisCommand(_ctx, "AUTH %s", _passwd.c_str()));
		if (reply == NULL){
			SLASSERT(false, "redis auth failed");
			redisFree(_ctx);
			return false;
		}
			
		if (REDIS_REPLY_ERROR == reply->type || REDIS_REPLY_NIL == reply->type) {
			SLASSERT(false, "error:%s", reply->str);
			freeReplyObject(reply);
			redisFree(_ctx);
			return false;
		}

		freeReplyObject(reply);
	}

	return true;
}

bool SLRedisConnection::isActive(){
	if (!_ctx || _ctx->err != REDIS_OK)
		return false;

	redisReply *reply = (redisReply*)redisCommand(_ctx, "ping");
	if (reply == NULL) 
		return false;

	if (reply->type != REDIS_REPLY_STATUS) 
		return false;

	if (_stricmp(reply->str, "PONG") != 0) 
		return false;

	freeReplyObject(reply);
	return true;
}

bool SLRedisConnection::ping(){
	if (isActive())
		return true;
	return reconnect();
}

bool SLRedisConnection::exec(char* command, const int32 size, const std::function<bool(ISLRedisResult* result)>& f){
	if (!ping()){
		SLASSERT(false, "connect redis failed");
		return false;
	}

	redisReply * reply = NULL;
	__redisAppendCommand(_ctx, command, size);
	
	struct timeval tv = { 0, 1000 };
	int32 setRet = redisSetTimeout(_ctx, tv);
	if(setRet != REDIS_OK){
		SLASSERT(false, "wtf");
		return false;
	}

	redisGetReply(_ctx, (void**)&reply);
	if (NULL == reply) {
		return false;
	}
	
	if (REDIS_REPLY_ERROR == reply->type || REDIS_REPLY_NIL == reply->type) {
		SLASSERT(false, "error:%s", reply->str);
		freeReplyObject(reply);
		return false;
	}

	bool ret = true;
	if (f) {
		SLRedisCallResult rst(reply);
		ret = f(&rst);
	}
	freeReplyObject(reply);

	return ret;
}

}
}
