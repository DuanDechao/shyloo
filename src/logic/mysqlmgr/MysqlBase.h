#ifndef __SL_CORE_MYSQL_BASE_H__
#define __SL_CORE_MYSQL_BASE_H__
#include "IMysqlMgr.h"
#include "sldb.h"
using namespace sl::db;
class MysqlResult : public IMysqlResult{
public:
	MysqlResult():_errCode(0){}
	virtual ~MysqlResult() {}

	virtual int32 rowCount() const { return (int32)_result.size(); }
	virtual int32 columnCount() const { return (int32)_columns.size(); }
	virtual bool columnExist(const char* column) const { return _columns.find(column) != _columns.end(); }
	virtual int8 getDataInt8(const int32 i, const char* key) const { return sl::CStringUtils::StringAsInt8(getColData(i, key).c_str()); }
	virtual int16 getDataInt16(const int32 i, const char* key) const { return sl::CStringUtils::StringAsInt16(getColData(i, key).c_str()); }
	virtual int32 getDataInt32(const int32 i, const char* key) const { return sl::CStringUtils::StringAsInt32(getColData(i, key).c_str()); }
	virtual int64 getDataInt64(const int32 i, const char* key) const { return sl::CStringUtils::StringAsInt64(getColData(i, key).c_str()); }
	virtual const char* getDataString(const int32 i, const char* key) const { return getColData(i, key).c_str(); }
	virtual int32 errCode() {return _errCode;}
	virtual const char* errInfo() {return _errInfo.c_str();}
	virtual void release() {DEL this;}
	void setErrCode(int32 code) {_errCode = code;}
	void setErrInfo(const char* info) {_errInfo = info;}

	void setColData(ISLDBResult* dbResult){
		int32 fieldNum = (int32)dbResult->fieldNum();
		std::vector<string> rowResult;
		for (int32 i = 0; i < fieldNum; i++){
			rowResult.push_back(string(dbResult->fieldValue(i), dbResult->fieldLength(i) + 1));
		}
		_result.push_back(std::move(rowResult));
	}

	void setColumns(ISLDBResult* dbResult){
		if (!_columns.empty())
			return;

		int32 fieldNum = (int32)dbResult->fieldNum();
		for (int32 i = 0; i < fieldNum; i++){
			_columns.insert(make_pair(dbResult->fieldName(i), i));
		}
	}

private:
	inline int32 columnIdx(const char* column) const{
		auto itor = _columns.find(column);
		SLASSERT(itor != _columns.end(), "cannot find column %s", column);
		return itor->second;
	}

	const std::string& getColData(int32 row, const char* column) const{
		SLASSERT(row >= 0 && row < (int32)_result.size(), "out of range");
		const int32 idx = columnIdx(column);
		return _result[row][idx];
	}

private:
	std::unordered_map<string, int32> _columns;
	std::vector<std::vector<string>> _result;
	int32 _errCode;
	std::string _errInfo;
};

class MysqlBase : public IMysqlBase, public sl::api::IAsyncHandler{
public:
	MysqlBase(ISLDBConnection* dbConn, SQLCommand* sqlCommand);
	~MysqlBase();

	virtual bool onExecute(sl::api::IKernel* pKernel);
	virtual bool onSuccess(sl::api::IKernel* pKernel);
	virtual bool onFailed(sl::api::IKernel* pKernel, bool nonviolent);
	virtual void onRelease(sl::api::IKernel* pKernel);

	void Exec(IMysqlHandler* handler);
	
	static int32 realExecSql(SQLCommand* sqlCommand, ISLDBConnection* dbConnection, MysqlResult* mysqlResult);
	static int32 getTableFields(ISLDBConnection* dbConnection, const char* tableName, MysqlResult* mysqlResult);
private:
	ISLDBConnection* _dbConnection;
	IMysqlHandler* _handler;
	SQLCommand* _sqlCommand;
	MysqlResult _result;
	int32 _errCode;
	int32 _affectedRow;
};
#endif
