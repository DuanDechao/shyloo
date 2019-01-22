#include "sldb_result.h"

namespace sl{
namespace db{

sl::SLPool<SLDBResult> SLDBResult::s_pool;
SLDBResult::SLDBResult()
	:_result(NULL),
	_fields(NULL),
	_currRow(NULL),
	_curRowfieldLengths(NULL),
	_fieldNum(0),
	_rowNum(0)
{}

SLDBResult::~SLDBResult(){
	if (_result){
		mysql_free_result(_result);
		_result = NULL;
		_fields = NULL;
		_currRow = NULL;
		_curRowfieldLengths = NULL;
		_fieldNum = 0;
		_rowNum = 0;
	}
}


void SLDBResult::setResult(MYSQL& mysql){
	setResult(mysql_store_result(&mysql));
}

void SLDBResult::setResult(MYSQL_RES* result){
	_result = result;
	if (_result){
		_fields = mysql_fetch_fields(_result);
		_fieldNum = mysql_num_fields(_result);
		_rowNum = (unsigned int)mysql_num_rows(_result);
	}
}

bool SLDBResult::next(){
	if (_result){
		_currRow = mysql_fetch_row(_result);
		if (_currRow){
			_curRowfieldLengths = mysql_fetch_lengths(_result);
			return true;
		}
	}
	return false;
}

void SLDBResult::release(){
	DEL this;
	//s_pool.recover(this);
}

int8 SLDBResult::toInt8(const int32 index){
	if (index >=0 && index < (int32)_fieldNum){
		if (_currRow && _currRow[index])
			return (int8)atoi(_currRow[index]);
	}
	return 0;
}

uint8 SLDBResult::toUint8(const int32 index){
	if (index >= 0 && index < (int32)_fieldNum){
		if (_currRow && _currRow[index])
			return (uint8)atoi(_currRow[index]);
	}
	return 0;
}

int16 SLDBResult::toInt16(const int32 index){
	if (index >= 0 && index < (int32)_fieldNum){
		if (_currRow && _currRow[index])
			return (int16)atoi(_currRow[index]);
	}
	return 0;
}

uint16 SLDBResult::toUint16(const int32 index){
	if (index >= 0 && index < (int32)_fieldNum){
		if (_currRow && _currRow[index])
			return (uint16)atoi(_currRow[index]);
	}
	return 0;
}

int32 SLDBResult::toInt32(const int32 index){
	if (index >= 0 && index < (int32)_fieldNum){
		if (_currRow && _currRow[index])
			return (int32)atoi(_currRow[index]);
	}
	return 0;
}

uint32 SLDBResult::toUint32(const int32 index){
	if (index >= 0 && index < (int32)_fieldNum){
		if (_currRow && _currRow[index])
			return (uint32)atoll(_currRow[index]);
	}
	return 0;
}

int64 SLDBResult::toInt64(const int32 index){
	if (index >= 0 && index < (int32)_fieldNum){
		if (_currRow && _currRow[index])
			return (int64)atol(_currRow[index]);
	}
	return 0;
}

uint64 SLDBResult::toUint64(const int32 index){
	if (index >= 0 && index < (int32)_fieldNum){
		if (_currRow && _currRow[index])
			return (uint64)strtoul(_currRow[index], NULL, 10);
	}
	return 0;
}

float SLDBResult::toFloat(const int32 index){
	if (index >= 0 && index < (int32)_fieldNum){
		if (_currRow && _currRow[index])
			return (float)atof(_currRow[index]);
	}
	return 0.0;
}

const char* SLDBResult::toString(const int32 index){
	if (index >= 0 && index < (int32)_fieldNum){
		if (_currRow && _currRow[index])
			return (const char*)_currRow[index];
	}
	return "";
}

const char* SLDBResult::fieldName(const int32 index) const{
	if (index >= 0 && index < (int32)_fieldNum){
		if (_fields){
			return _fields[index].name;
		}
	}
	return "";
}

const char* SLDBResult::fieldValue(const int32 index) const{
	if (index >= 0 && index < (int32)_fieldNum){
		if (_currRow && _currRow[index]){
			return (const char*)_currRow[index];
		}
	}
	return "";
}

unsigned long SLDBResult::fieldLength(const int32 index) const{
	if (index >= 0 && index < (int32)_fieldNum){
		if (_curRowfieldLengths){
			return _curRowfieldLengths[index];
		}
	}
	return 0;
}

unsigned long SLDBResult::fieldDBLength(const int32 index) const{
	if(index >= 0 && index < (int32)_fieldNum){
		if(_fields){
			return _fields[index].length;
		}
	}
	return 0;
}

unsigned long SLDBResult::fieldDBMaxLength(const int32 index) const{
	if(index >= 0 && index < (int32)_fieldNum){
		if(_fields){
			return _fields[index].max_length;
		}
	}
	return 0;
}

unsigned int SLDBResult::fieldFlags(const int32 index) const{
	if(index >= 0 && index < (int32)_fieldNum){
		if(_fields){
			return _fields[index].flags;
		}
	}
	return 0;
}

int	SLDBResult::fieldType(const int32 index) const{
	if(index >= 0 && index < (int32)_fieldNum){
		if(_fields){
			return _fields[index].type;
		}
	}
	return 0;
}


}
}
