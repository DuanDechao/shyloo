#ifndef __SL_DB_REUSLT_H__
#define __SL_DB_REUSLT_H__
#include "sldb_define.h"
#include "slobjectpool.h"
#include <unordered_map>
namespace sl
{
namespace db
{
class SLDBResult : public ISLDBResult{
public:
	SLDBResult();
	~SLDBResult();
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

	void setResult(MYSQL& mysql);

	virtual unsigned int SLAPI filedNum() const { return m_filedNum; }
	virtual unsigned int SLAPI rowNum() const { return m_rowNum; }
	virtual int32 SLAPI colNameToIdx(const char* colName) const{
		auto itor = m_colNameIdxMap.find(colName);
		if (itor == m_colNameIdxMap.end())
			return -1;
		return itor->second;
	}
private:
	MYSQL_RES*		m_result;
	MYSQL_FIELD*	m_fields;
	MYSQL_ROW		m_currRow;

	unsigned long*	m_curRowfieldLengths;
	unsigned int	m_filedNum;
	unsigned int    m_rowNum;
	std::unordered_map<std::string, int32> m_colNameIdxMap;
};

CREATE_OBJECT_POOL(SLDBResult);

}
}
#endif
