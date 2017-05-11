#ifndef __SL_INTERFACE_DBTYPE_H__
#define __SL_INTERFACE_DBTYPE_H__
#include "slmulti_sys.h"
#include "sltools.h"
#include "slstring_utils.h"
struct DBType{
	enum{
		DBTYPE_INT8 = 0,
		DBTYPE_INT16,
		DBTYPE_INT32,
		DBTYPE_INT64,
		DBTYPE_FLOAT,
		DBTYPE_STRING,
		DBTYPE_BLOB,
		DBTYPE_TIME,
		DBTYPE_TEXT,
		DBTYPE_BIT,
	};

	DBType& convert(const char* typeStr, const char* size = nullptr){
		SLASSERT(typeStr, "wtf");
		char type[32] = { 0 };
		char typeSize[32] = { 0 };
		const char* start = typeStr;
		const char* pos = strstr(typeStr, "(");
		if (pos != nullptr){
			sl::SafeMemcpy(type, sizeof(type)-1, typeStr, pos - start);
			const char* end = strstr(typeStr, ")");
			sl::SafeMemcpy(typeSize, sizeof(typeSize)-1, pos + 1, end - pos);
		}
		else{
			sl::SafeMemcpy(type, sizeof(type)-1, typeStr, strlen(typeStr));
		}

		if (strcmp(type, "int8") == 0 || strcmp(type, "tinyint") == 0){
			_dbType = DBTYPE_INT8;
			_typeSize = 4;
		}
		else if (strcmp(type, "int16") == 0 || strcmp(type, "smallint") == 0){
			_dbType = DBTYPE_INT16;
			_typeSize = 6;
		}
		else if (strcmp(type, "int32") == 0 || strcmp(type, "int") == 0){
			_dbType = DBTYPE_INT32;
			_typeSize = 11;
		}
		else if (strcmp(type, "int64") == 0 || strcmp(type, "bigint") == 0){
			_dbType = DBTYPE_INT64;
			_typeSize = 20;
		}
		else if (strcmp(type, "string") == 0 || strcmp(type, "varchar") == 0){
			_dbType = DBTYPE_STRING;
			if (size == nullptr && strcmp(typeSize, "") != 0){
				_typeSize = sl::CStringUtils::StringAsInt32(typeSize);
			}
			else if (size != nullptr){
				_typeSize = sl::CStringUtils::StringAsInt32(size);
			}
		}
		else if (strcmp(type, "float") == 0){
			_dbType = DBTYPE_FLOAT;
		}
		else if (strcmp(type, "blob") == 0){
			_dbType = DBTYPE_BLOB;
		}
		else if (strcmp(type, "timestamp") == 0 || strcmp(type, "datatime") == 0){
			_dbType = DBTYPE_TIME;
		}
		else if (strcmp(type, "text") == 0){
			_dbType = DBTYPE_TEXT;
		}
		else if (strcmp(type, "bit") == 0){
			_dbType = DBTYPE_BIT;
			_typeSize = 1;
			if (size == nullptr && strcmp(typeSize, "") == 0){
				_typeSize = sl::CStringUtils::StringAsInt32(typeSize);
			}
			else if (size != nullptr){
				_typeSize = sl::CStringUtils::StringAsInt32(size);
			}
		}
		else{
			SLASSERT(false, "wtf");
		}
		return *this;
	}

	const char* c_str(){
		sl::SafeMemset(_typeStr, sizeof(_typeStr), 0, sizeof(_typeStr));
		switch (_dbType)
		{
		case DBTYPE_INT8:
			SafeSprintf(_typeStr, sizeof(_typeStr), "tinyint(%d)", _typeSize);
			break;
		case DBTYPE_INT16:
			SafeSprintf(_typeStr, sizeof(_typeStr), "smallint(%d)", _typeSize);
			break;
		case DBTYPE_INT32:
			SafeSprintf(_typeStr, sizeof(_typeStr), "int(%d)", _typeSize);
			break;
		case DBTYPE_INT64:
			SafeSprintf(_typeStr, sizeof(_typeStr), "bigint(%d)", _typeSize);
			break;
		case DBTYPE_STRING:
			SafeSprintf(_typeStr, sizeof(_typeStr), "varchar(%d)", _typeSize);
			break;
		case DBTYPE_FLOAT:
			SafeSprintf(_typeStr, sizeof(_typeStr), "float");
			break;
		case DBTYPE_BLOB:
			SafeSprintf(_typeStr, sizeof(_typeStr), "blob");
			break;
		case DBTYPE_TIME:
			SafeSprintf(_typeStr, sizeof(_typeStr), "timestamp");
			break;
		case DBTYPE_TEXT:
			SafeSprintf(_typeStr, sizeof(_typeStr), "text");
			break;
		case DBTYPE_BIT:
			SafeSprintf(_typeStr, sizeof(_typeStr), "bit(%d)", _typeSize);
			break;
		default:
			SLASSERT(false, "wtf");
			break;
		}
		return _typeStr;
	}

	inline bool operator == (const DBType& right) const{
		if (right._dbType == this->_dbType && right._typeSize == this->_typeSize){
			return true;
		}
		else{
			return false;
		}
	}

	inline bool operator != (const DBType& right) const{
		return *this == right ? false : true;
	}

	int32 _dbType;
	int32 _typeSize;
	char _typeStr[32];

};
#endif