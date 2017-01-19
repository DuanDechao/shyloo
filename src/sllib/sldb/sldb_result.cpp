#include "sldb_result.h"

namespace sl
{
namespace db
{
SLDBResult::SLDBResult()
	:m_result(NULL),
	m_fields(NULL),
	m_currRow(NULL),
	m_curRowfieldLengths(NULL),
	m_filedNum(0),
	m_rowNum(0)
{}

SLDBResult::~SLDBResult(){
	if (m_result){
		mysql_free_result(m_result);
		m_result = NULL;
		m_fields = NULL;
		m_currRow = NULL;
		m_curRowfieldLengths = NULL;
		m_filedNum = 0;
		m_rowNum = 0;
	}
}


void SLDBResult::setResult(MYSQL& mysql){
	m_result = mysql_store_result(&mysql);
	if (m_result){
		m_fields = mysql_fetch_fields(m_result);
		m_filedNum = mysql_num_fields(m_result);
		m_rowNum = (unsigned int)mysql_num_rows(m_result);
	}
}

bool SLDBResult::next(){
	if (m_result){
		m_currRow = mysql_fetch_row(m_result);
		if (m_currRow){
			m_curRowfieldLengths = mysql_fetch_lengths(m_result);
			return true;
		}
	}
	return false;
}

void SLDBResult::release(){
	RELEASE_POOL_OBJECT(SLDBResult, this);
}

int8 SLDBResult::toInt8(const int32 index){
	if (index >=0 && index < (int32)m_filedNum){
		if (m_currRow && m_currRow[index])
			return (int8)atoi(m_currRow[index]);
	}
	return 0;
}

uint8 SLDBResult::toUint8(const int32 index){
	if (index >= 0 && index < (int32)m_filedNum){
		if (m_currRow && m_currRow[index])
			return (uint8)atoi(m_currRow[index]);
	}
	return 0;
}

int16 SLDBResult::toInt16(const int32 index){
	if (index >= 0 && index < (int32)m_filedNum){
		if (m_currRow && m_currRow[index])
			return (int16)atoi(m_currRow[index]);
	}
	return 0;
}

uint16 SLDBResult::toUint16(const int32 index){
	if (index >= 0 && index < (int32)m_filedNum){
		if (m_currRow && m_currRow[index])
			return (uint16)atoi(m_currRow[index]);
	}
	return 0;
}

int32 SLDBResult::toInt32(const int32 index){
	if (index >= 0 && index < (int32)m_filedNum){
		if (m_currRow && m_currRow[index])
			return (int32)atoi(m_currRow[index]);
	}
	return 0;
}

uint32 SLDBResult::toUint32(const int32 index){
	if (index >= 0 && index < (int32)m_filedNum){
		if (m_currRow && m_currRow[index])
			return (uint32)atoll(m_currRow[index]);
	}
	return 0;
}

int64 SLDBResult::toInt64(const int32 index){
	if (index >= 0 && index < (int32)m_filedNum){
		if (m_currRow && m_currRow[index])
			return (int64)atol(m_currRow[index]);
	}
	return 0;
}

uint64 SLDBResult::toUint64(const int32 index){
	if (index >= 0 && index < (int32)m_filedNum){
		if (m_currRow && m_currRow[index])
			return (uint64)strtoul(m_currRow[index], NULL, 10);
	}
	return 0;
}

float SLDBResult::toFloat(const int32 index){
	if (index >= 0 && index < (int32)m_filedNum){
		if (m_currRow && m_currRow[index])
			return (float)atof(m_currRow[index]);
	}
	return 0.0;
}

const char* SLDBResult::toString(const int32 index){
	if (index >= 0 && index < (int32)m_filedNum){
		if (m_currRow && m_currRow[index])
			return (const char*)m_currRow[index];
	}
	return NULL;
}


}
}