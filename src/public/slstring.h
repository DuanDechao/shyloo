//定长的字符串封装
#ifndef _SL_SIZE_STRING_H_
#define _SL_SIZE_STRING_H_
#include "slmulti_sys.h"
#include <stdio.h>
#include <string>
#include <stdarg.h>
namespace sl
{
//定长的字符串封装类
template<int SIZE = 1024>
class SLString
{
public:
	SLString(){
		m_szBuf[0] = 0;
	}

	SLString(const char* str){
		*this = str;
	}

	SLString(const std::string str){
		*this = str;
	}

	template<int S>
	SLString(const SLString<S>& str){
		*this = str;
	}

	virtual ~SLString(){
		m_szBuf[0] = 0;
	}

	const char* c_str() const{
		return m_szBuf;
	}

	const char& operator[] (int i) const{
		return m_szBuf[i];
	}

	char& operator[] (int i){
		return m_szBuf[i];
	}

	SLString& operator = (const char* str) {
		SafeSprintf(m_szBuf, SIZE, "%s", str);
		m_szBuf[SIZE - 1] = 0;
		return *this;
	}

	SLString& operator = (const std::string& str) {
		return *this = str.c_str();
	}

	template<int S>
	SLString& operator = (const SLString<S>& szString){
		return *this = szString.c_str();
	}

	SLString& operator << (const char* str){
		int iLen = length();
		int iMaxLen = Size() - iLen;
		iMaxLen = iMaxLen < 0 ? 0 : iMaxLen;
		SafeSprintf(m_szBuf + iLen, iMaxLen, "%s", str);
		m_szBuf[SIZE - 1] = 0;
		return *this;
	}

	SLString& operator << (const std::string str){
		return *this << str.c_str();
	}

	template<int S>
	SLString& operator << (const SLString<S>& szString){
		return *this << szString.c_str();
	}

	virtual int length() const{
		return (int)strlen(m_szBuf);
	}

	virtual int Size() const{
		return sizeof(m_szBuf);
	}

	void Clear(){
		SafeMemset(m_szBuf, sizeof(m_szBuf), 0, SIZE);
	}

	bool operator == (const char* str) const{
		return strcmp(m_szBuf, str) == 0;
	}

	bool operator == (const std::string str) const{
		return *this == str.c_str();
	}

	template<int S>
	bool operator == (const SLString<S>& str) const{
		return *this == str.c_str();
	}

	bool operator >= (const char* pstr) const
	{
		return strcmp(m_szBuf, pstr) >= 0;
	}

	bool operator >= (const std::string str) const{
		return *this >= str.c_str();
	}

	template<int S>
	bool operator >= (const SLString<S>& str) const{
		return *this >= str.c_str();
	}

	bool operator <= (const char* pstr) const
	{
		return strcmp(m_szBuf, pstr) <= 0;
	}

	bool operator <= (const std::string str) const{
		return *this <= str.c_str();
	}

	template<int S>
	bool operator <= (const SLString<S>& str) const{
		return *this <= str.c_str();
	}

	bool operator < (const char* pstr) const
	{
		return strcmp(m_szBuf, pstr) >= 0;
	}

	bool operator < (const std::string str) const{
		return *this < str.c_str();
	}

	template<int S>
	bool operator < (const SLString<S>& str) const{
		return *this < str.c_str();
	}

	bool operator < (const SLString<SIZE>& str) const{
		return *this < str.c_str();
	}

	bool operator > (const char* pstr) const{
		return strcmp(m_szBuf, pstr) > 0;
	}

	bool operator > (const std::string str) const{
		return *this > str.c_str();
	}

	template<int S>
	bool operator > (const SLString<S>& str) const{
		return *this > str.c_str();
	}

	bool operator != (const char* pstr) const{
		return strcmp(m_szBuf, pstr) != 0;
	}

	bool operator != (const std::string str) const{
		return *this != str.c_str();
	}

	template<int S>
	bool operator != (const SLString<S>& str) const{
		return *this != str.c_str();
	}

private:
	char    m_szBuf[SIZE];

};// class SLString

template<int SIZE>
struct HashFunc
{
	size_t operator()(const SLString<SIZE> &str) const{
		unsigned long __h = 0;
		for (size_t i = 0; i < (size_t)str.length(); i++)
			__h = 5 * __h + str[i];
		return size_t(__h);
	}
};

template<int SIZE>
struct EqualFunc
{
	bool operator()(const SLString<SIZE> &lstr, const SLString<SIZE> &rstr) const{
		return lstr == rstr;
	}
};

} //namespace sl
#endif