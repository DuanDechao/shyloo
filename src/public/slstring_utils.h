//shyloo字符串常用函数封装类
#ifndef _SL_STRING_UTILS_
#define _SL_STRING_UTILS_
#include "slmulti_sys.h"
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
namespace sl
{
//字符串常用函数类
class CStringUtils{
public:
	/*
		删除字符串中某些字符串外的字符
		@return	 返回删除的次数
	*/
	static int RemoveNot(std::string& str, const char* pszTrim){

		size_t iIndex = str.find_first_not_of(pszTrim);
		int iCount = 0;
		while(iIndex != std::string::npos)
		{
			str.erase(iIndex, 1);
			iIndex = str.find_first_not_of(pszTrim);
			++iCount;
		}
		return iCount;
	}

	/*
		分割字符串的相关操作
		把字符串分割成多个字符串，可以有多个分割符
		@note 这种方式会忽略重复的分隔符
	*/
	static void Split(const string& strBuf, const string& strDel, std::vector<std::string>& aryStringList){
		size_t pos1 = 0, pos2 = 0;
		while(true)
		{
			pos1 = strBuf.find_first_not_of(strDel, pos2);
			if(pos1 == string::npos)
			{
				break;
			}
			pos2 = strBuf.find_first_of(strDel, pos1);
			if(pos2 == string::npos)
			{
				aryStringList.push_back(strBuf.substr(pos1));
				break;
			}
			else
			{
				aryStringList.push_back(strBuf.substr(pos1, pos2-pos1));
			}
		}
	}

	/*
		分割name = value 串
		@return 分割成功返回true
	*/
	static bool SplitIni(const string& strLine, string& strName, string& strValue, char chDel = '='){
		if(strLine.size() == 0)
		{
			return false;
		}

		size_t iIndex = strLine.find(chDel);
		if(iIndex == string::npos)
		{
			return false;
		}
		strName = strLine.substr(0, iIndex);
		strValue = strLine.substr(iIndex + 1, strLine.size() - iIndex -1);
			
		return true;
	}

	///删除字符串左边中的某些字符
	static string& TrimLeft(string& str, const char* pszTrim){
		size_t iIndex = str.find_first_not_of(pszTrim);
		if(iIndex != string::npos)
		{
			str.erase(0, iIndex);
		}

		return str;
	}

	//删除字符串右边中的某些字符
	static string& TrimRight(string& str, const char* pszTrim){
		size_t iIndex = str.find_last_not_of(pszTrim);
		if((++iIndex) != str.length())
		{
			str.erase(iIndex, str.length() - iIndex);
		}

		return str;
	}

	//转为小写
	static string& MakeLower(string& str){
		transform(str.begin(), str.end(), str.begin(), static_cast<int(*)(int)>(tolower));
		return str;
	}

	static string & RepleaceAll(string& str, const string& oldstr, const string& newstr){
		do{
			string::size_type pos(0);
			if((pos = str.find(oldstr)) != string::npos){
				str.replace(pos, oldstr.length(), newstr);
			}
			else{
				break;
			}
		}while(true);
		return str;
	}

		
	static int32 StringAsInt32(const char* pStr){
		SLASSERT(pStr, "is null pointer");
		return atoi(pStr);
	}

	static string Int32AsString(int32 val){
		char str[128] ={0};
		SafeSprintf(str, 127, "%d", val);
		return str;
	}

	static float StringAsFloat(const char* pstr){
		SLASSERT(pstr, "is null pointer");
		return (float)atof(pstr);
	}

	static string FloatAsString(float val){
		char str[128] ={0};
		SafeSprintf(str, 127, "%f", val);
		return str;
	}

	static int64 StringAsInt64(const char* pstr){
		SLASSERT(pstr, "is null pointer");
		return atoll(pstr);
	}

	static string Int64AsString(int64 val){
		char str[128] ={0};
		SafeSprintf(str, 127, "%lld", val);
		return str;
	}

	static int16 StringAsInt16(const char* pStr){
		SLASSERT(pStr, "is null pointer");
		return (int16)StringAsInt32(pStr);
	}

	static string Int16AsString(int16 val){
		char str[128] = { 0 };
		SafeSprintf(str, 127, "%d", val);
		return str;
	}

	static int8 StringAsInt8(const char* pStr){
		SLASSERT(pStr, "is null pointer");
		return (int8)StringAsInt32(pStr);
	}

	static string Int8AsString(int16 val){
		char str[128] = { 0 };
		SafeSprintf(str, 127, "%d", val);
		return str;
	}

	static bool StringAsBoolean(const char* pStr){
		SLASSERT(pStr, "is null pointer");
		string inputStr(pStr);
		if (strcmp(MakeLower(inputStr).c_str(), "true") == 0)
			return true;
		else if (strcmp(MakeLower(inputStr).c_str(), "false") == 0)
			return false;
		else{
			//SLASSERT(false, "invalid input string");
			return false;
		}
	}

	static const char* BooleanAsString(bool val){
		if (val)
			return "true";
		else
			return "false";
	}
};

}//namespace sl
#endif
