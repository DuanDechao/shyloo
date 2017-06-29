#ifndef __SL_DB_REUSLT_H__
#define __SL_DB_REUSLT_H__
#include "sldb_define.h"
#include <unordered_map>
#include "slpool.h"

namespace sl{
namespace db{

class SLDBResult : public ISLDBResult{
public:
	inline static SLDBResult* create(){
		return CREATE_FROM_POOL(s_pool);
	}
	
	virtual bool SLAPI next();
	virtual void SLAPI release();

	virtual int8 SLAPI toInt8(const int32 index);
	virtual uint8 SLAPI toUint8(const int32 index);
	virtual int16 SLAPI toInt16(const int32 index);
	virtual uint16 SLAPI toUint16(const int32 index);
	virtual int32 SLAPI toInt32(const int32 index);
	virtual uint32 SLAPI toUint32(const int32 index);
	virtual int64 SLAPI toInt64(const int32 index);
	virtual uint64 SLAPI toUint64(const int32 index);
	virtual float SLAPI toFloat(const int32 index);
	virtual const char* SLAPI toString(const int32 index);
	
	virtual unsigned int SLAPI fieldNum() const { return _filedNum; }
	virtual unsigned int SLAPI rowNum() const { return _rowNum; }
	virtual const char* SLAPI fieldName(const int32 index) const;
	virtual const char* SLAPI fieldValue(const int32 index) const;
	virtual unsigned long SLAPI fieldLength(const int32 index) const;

	void setResult(MYSQL& mysql);

private:
	friend sl::SLPool<SLDBResult>;

	SLDBResult();
	~SLDBResult();

private:
	MYSQL_RES*		_result;
	MYSQL_FIELD*	_fields;
	MYSQL_ROW		_currRow;

	unsigned long*	_curRowfieldLengths;
	unsigned int	_filedNum;
	unsigned int    _rowNum;

	static sl::SLPool<SLDBResult> s_pool;
};

}
}
#endif
