#ifndef SL_SLDB_H
#define SL_SLDB_H
#include "sltype.h"
namespace sl
{
namespace db
{
class ISLDBResult
{
public:
	virtual int8 SLAPI toInt8(const int32 index) = 0;
	virtual uint8 SLAPI toUint8(const int32 index) = 0;
	virtual int16 SLAPI toInt16(const int32 index) = 0;
	virtual uint16 SLAPI toUint16(const int32 index) = 0;
	virtual int32 SLAPI toInt32(const int32 index) = 0;
	virtual uint32 SLAPI toUint32(const int32 index) = 0;
	virtual int64 SLAPI toInt64(const int32 index) = 0;
	virtual uint64 SLAPI toUint64(const int32 index) = 0;
	virtual float SLAPI toFloat(const int32 index) = 0;
	virtual const char* SLAPI toString(const int32 index) = 0;
};

class ISLDBConnection
{
public:
	virtual bool SLAPI open(const char* szHostName, const int32 port, const char* szName, const char* szPwd, const char* szDBName, const char* szCharSet) = 0;
	virtual bool SLAPI reOpen() = 0;
	virtual bool SLAPI execute(const char* commandSql) = 0;
	virtual bool SLAPI execute(const char* commandSql, ISLDBResult& result) = 0;
	virtual int32 SLAPI getError(void) = 0;
	virtual const char* SLAPI getErrorInfo(void) = 0;
};

class ISLDBMgr
{
public:
	virtual ISLDBConnection* SLAPI newConnection(void) = 0;
};

ISLDBMgr* SLAPI getSLDBModule(void);

}
}
#endif