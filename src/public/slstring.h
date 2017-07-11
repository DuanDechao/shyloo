//定长的字符串封装
#ifndef _SL_SIZE_STRING_H_
#define _SL_SIZE_STRING_H_
#include "slmulti_sys.h"
#include <stdio.h>
#include <string>
#include <stdarg.h>
#include "sltools.h"
namespace sl{
//定长的字符串封装类
template<int16 SIZE = 64>
class SLString{
public:
	SLString(){
		SafeMemset(m_szBuf, sizeof(m_szBuf), 0, sizeof(m_szBuf));
	}

	SLString(const char* str){
		SafeMemset(m_szBuf, sizeof(m_szBuf), 0, sizeof(m_szBuf));

		int32 ilen = (int32)strlen(str);
		ilen = (ilen > SIZE) ? SIZE : ilen;
		safeMemcpy(m_szBuf, sizeof(m_szBuf), str, ilen);
	}

	inline const char* getString() const { return m_szBuf; }
	inline void clear() { SafeMemset(m_szBuf, sizeof(m_szBuf), 0, sizeof(m_szBuf)); }
	inline const char* c_str() const{return m_szBuf;}
	inline const char& operator[] (int i) const{ return m_szBuf[i]; }
	inline char& operator[] (int i){return m_szBuf[i]; }
	inline void assign(const char* str, const int32 ilen){ safeMemcpy(m_szBuf, sizeof(m_szBuf), str, (ilen > SIZE) ? (SIZE) : (ilen)); }
	inline int32 length() const { return (int32)strlen(m_szBuf); }

	bool operator == (const char* str) const { return !strcmp(m_szBuf, str); }
	bool operator != (const char* str) const { return !(*this == str); }
	bool operator == (const SLString& buff) const { return !strcmp(m_szBuf, buff.m_szBuf); }
	bool operator != (const SLString& buff) const { return strcmp(m_szBuf, buff.m_szBuf); }
	bool operator < (const SLString& buff) const { return (strcmp(m_szBuf, buff.m_szBuf)) < 0; }


	SLString& operator = (const char* str){
		SafeMemset(m_szBuf, sizeof(m_szBuf), 0, sizeof(m_szBuf));

		int32 ilen = (int32)strlen(str);
		ilen = (ilen > SIZE) ? SIZE : ilen;
		safeMemcpy(m_szBuf, sizeof(m_szBuf), str, ilen);
		return *this;
	}

	SLString& operator = (const int32 value){
		SafeSprintf(m_szBuf, sizeof(m_szBuf), "%d", value);
		return *this;
	}

	SLString& operator = (const int64 value){
		SafeSprintf(m_szBuf, sizeof(m_szBuf), "%lld", value);
		return *this;
	}

	SLString& operator = (const SLString & buff){
		int32 ilen = (int32)strlen(buff.getString());
		ilen = (ilen > SIZE) ? (SIZE) : ilen;
		safeMemcpy(m_szBuf, sizeof(m_szBuf), buff.m_szBuf, ilen);
		return *this;
	}

	//SLString(const SLString & buff){
	//	int32 ilen = (int32)strlen(buff.getString());
	//	ilen = (ilen > SIZE) ? (SIZE) : ilen;
	//	safeMemcpy(m_szBuf, sizeof(m_szBuf), buff.m_szBuf, ilen);
	//}

	SLString& operator << (const char* str){
		int32 ilen = (int32)strlen(m_szBuf);
		SafeSprintf(m_szBuf + ilen, sizeof(m_szBuf)-ilen, "%s", str);
		return *this;
	}

	SLString& operator << (const int32 value){
		int32 ilen = (int32)strlen(m_szBuf);
		SafeSprintf(m_szBuf + ilen, sizeof(m_szBuf)-ilen, "%d", value);
		return *this;
	}

	SLString& operator << (const int64 value){
		int32 ilen = (int32)strlen(m_szBuf);
		SafeSprintf(m_szBuf + ilen, sizeof(m_szBuf)-ilen, "%lld", value);
		return *this;
	}

	SLString& operator << (const float value){
		int32 ilen = (int32)strlen(m_szBuf);
		SafeSprintf(m_szBuf + ilen, sizeof(m_szBuf)-ilen, "%.2f", value);
		return *this;
	}

	SLString& operator << (char value){
		int32 ilen = (int32)strlen(m_szBuf);
		m_szBuf[ilen] = value;
		m_szBuf[ilen + 1] = 0;
		return *this;
	}

	operator size_t() const{
		size_t hash = 0;
		int32 count = (int32)strlen(m_szBuf);
		for (int32 i = 0; i < count + 1; i++)
			hash = 33 * hash + (size_t)m_szBuf[i];
		return hash;
	}

private:
	char    m_szBuf[SIZE + 1];

};// class SLString

} //namespace sl

namespace std{
	template<int16 size>
	struct hash<sl::SLString<size>>{
		inline size_t operator()(const sl::SLString<size>& key) const{
			return (size_t)key;
		}
	};
}
#endif