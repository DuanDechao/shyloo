///shyloo的时间类
/********************************************************************
	created:	2015/12/04
	created:	4:12:2015   16:47
	filename: 	e:\myproject\shyloo\sllib\sltime.h
	file path:	e:\myproject\shyloo\sllib
	file base:	sltime
	file ext:	h
	author:		ddc
	
	purpose:	时间封装类
*********************************************************************/

#ifndef _SL_TIME_H_
#define _SL_TIME_H_
#include "slmulti_sys.h"

#include <chrono>
#include <string>
#include <time.h>
#define CTIME_SECONDS_PRE_DAY		86400			///< 每天最大的秒数

namespace sl{
	///时间相关定义
	enum EnumTimeDef{
		ETD_TIME_ZERO	=	0*3600,					///< 0点钟
		ETD_TIME_ONE	=	1*3600,					///< 1点钟
		ETD_TIME_TWO	=	2*3600,					///< 2点钟
		ETD_TIME_THREE	=	3*3600,					///< 3点钟
		ETD_TIME_FOUR	=	4*3600,					///< 4点钟
		ETD_TIME_FIVE	=	5*3600,					///< 5点钟
		ETD_TIME_SIX	=	6*3600,					///< 6点钟
		ETD_TIME_SEVEN	=	7*3600,					///< 7点钟
		ETD_TIME_EIGHT	=	8*3600,					///< 8点钟
		ETD_TIME_NINE	=	9*3600,					///< 9点钟
		ETD_TIME_TEN	=	10*3600,				///< 10点钟
	};

	//时间跨度类
	class CTimeSpan{
	public:
		CTimeSpan() {}
		CTimeSpan(time_t time) { m_timeSpan = time;}
		CTimeSpan(long lDays, int iHours, int iMins, int iSecs) {m_timeSpan = iSecs + 60 *( iMins + 60*( iHours + 24 * lDays));}

		CTimeSpan(const CTimeSpan& timeSpanSrc) {m_timeSpan = timeSpanSrc.m_timeSpan;}
		const CTimeSpan& operator=(const CTimeSpan& timeSpanSrc){m_timeSpan = timeSpanSrc.m_timeSpan; return *this;}

		int GetDays() const {return static_cast<int>(m_timeSpan / (24 * 3600));}
		int GetHours() const {return static_cast<int> (GetTotalHours() - GetDays() * 24);}
		int GetMinutes() const {return static_cast<int>(GetTotalMinutes() - GetTotalHours() * 60);}
		int GetSeconds() const {return static_cast<int>(GetTotalSeconds() - GetTotalMinutes()* 60);}

		int GetTotalHours() const {return static_cast<int>(m_timeSpan / 3600);}
		int GetTotalMinutes() const {return static_cast<int>(m_timeSpan / 60);}
		int GetTotalSeconds() const {return static_cast<int>(m_timeSpan);}

		CTimeSpan operator-(CTimeSpan timeSpan) const {return CTimeSpan(m_timeSpan - timeSpan.m_timeSpan);}
		CTimeSpan operator+(CTimeSpan timeSpan) const {return CTimeSpan(m_timeSpan + timeSpan.m_timeSpan);}
		const CTimeSpan& operator+=(CTimeSpan timeSpan) {m_timeSpan += timeSpan.m_timeSpan; return *this;}
		const CTimeSpan& operator-=(CTimeSpan timeSpan) {m_timeSpan -= timeSpan.m_timeSpan; return *this;}
		bool operator==(CTimeSpan timeSpan) const {return m_timeSpan == timeSpan.m_timeSpan;}
		bool operator!=(CTimeSpan timeSpan) const {return m_timeSpan != timeSpan.m_timeSpan;}
		bool operator<(CTimeSpan timeSpan) const {return m_timeSpan < timeSpan.m_timeSpan;}
		bool operator>(CTimeSpan timeSpan) const {return m_timeSpan > timeSpan.m_timeSpan;}
		bool operator<=(CTimeSpan timeSpan) const {return m_timeSpan <= timeSpan.m_timeSpan;}
		bool operator>=(CTimeSpan timeSpan) const {return m_timeSpan >= timeSpan.m_timeSpan;}

		

		std::string Format(const char* pszFormat) const{
			char szBuffer[32];
			char ch = 0;
			std::string s;
			
			while((ch = *pszFormat++) != '\0'){
				if(ch == '%'){
					switch (ch = *pszFormat++){
					case '%':
						s += ch;
						break;
					case 'D':
						SafeSprintf(SL_STRSIZE(szBuffer), "%04d", GetDays());
						s += szBuffer;
						break;
					case 'H':
						SafeSprintf(SL_STRSIZE(szBuffer), "%02d", GetHours());
						s += szBuffer;
						break;
					case 'M':
						SafeSprintf(SL_STRSIZE(szBuffer), "%02d", GetMinutes());
						s+= szBuffer;
						break;
					case 'S':
						SafeSprintf(SL_STRSIZE(szBuffer), "%02d", GetSeconds());
						s+=szBuffer;
						break;
					default:
						return "";
					}
				}
				else{
					s += ch;
				}
			}

			return s;
		}
	private:
		time_t		m_timeSpan;
		friend		class CTime;
	};

	//时间类
	class CTime
	{
	public:
		CTime() {}
		CTime(time_t time) {m_time = time;}
		CTime(struct tm t){
			m_time = mktime(&t);
		}

		CTime(int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec, int iDST = -1){
			struct tm atm;
			atm.tm_sec   = iSec;
			atm.tm_min   = iMin;
			atm.tm_hour  = iHour;
			atm.tm_mday  = iDay;
			atm.tm_mon   = iMonth - 1;           ///< tm_mon is 0 based
			atm.tm_year  = iYear - 1900;        ///< tm_year is 1900 based
			atm.tm_isdst = iDST;
			m_time = mktime(&atm);

		}

		CTime(const std::string& strDateTime){
			struct tm stTm;
			const size_t iLen = strDateTime.length();
			if(iLen == 19){
				/// YYYY-mm-dd HH:MM:SS
				stTm.tm_year =  strtol(strDateTime.substr(0,4).c_str(), NULL, 10) - 1900;
				stTm.tm_mon  =  strtol(strDateTime.substr(5,2).c_str(), NULL, 10) - 1;
				stTm.tm_mday =  strtol(strDateTime.substr(8,2).c_str(), NULL, 10);
				stTm.tm_hour =  strtol(strDateTime.substr(11,2).c_str(), NULL, 10);
				stTm.tm_min  =  strtol(strDateTime.substr(14,2).c_str(), NULL, 10);
				stTm.tm_sec  =  strtol(strDateTime.substr(17,2).c_str(), NULL, 10);
				m_time = mktime(&stTm);
				if(m_time == -1)
				{
					m_time = 0;
				}
			}
			else if(iLen == 14) {
				/// YYYYmmddHHMMSS
				stTm.tm_year =  strtol(strDateTime.substr(0,4).c_str(), NULL, 10) - 1900;
				stTm.tm_mon  =  strtol(strDateTime.substr(4,2).c_str(), NULL, 10) - 1;
				stTm.tm_mday =  strtol(strDateTime.substr(6,2).c_str(), NULL, 10);
				stTm.tm_hour =  strtol(strDateTime.substr(8,2).c_str(), NULL, 10);
				stTm.tm_min  =  strtol(strDateTime.substr(10,2).c_str(), NULL, 10);
				stTm.tm_sec  =  strtol(strDateTime.substr(12,2).c_str(), NULL, 10);
				m_time = mktime(&stTm);
				if(m_time == -1){
					m_time = 0;
				}
			}
			else if(iLen == 10){
				//YYYY-mm-dd
				stTm.tm_year =  strtol(strDateTime.substr(0,4).c_str(), NULL, 10) - 1900;
				stTm.tm_mon  =  strtol(strDateTime.substr(5,2).c_str(), NULL, 10) - 1;
				stTm.tm_mday =  strtol(strDateTime.substr(8,2).c_str(), NULL, 10);
				stTm.tm_hour = 0;
				stTm.tm_min  = 0;
				stTm.tm_sec  = 0;
				m_time  = mktime(&stTm);
				if(m_time == -1){
					m_time = 0;
				}
			}
			else{
				m_time = 0;
			}
		}

		CTime(const CTime& timeSrc) {m_time = timeSrc.m_time;}

		const CTime& operator=(const CTime& timeSrc) {m_time = timeSrc.m_time; return *this;}
		const CTime& operator=(time_t t) {m_time = t; return *this;}

		struct tm* GetGmtTm(struct tm* ptm = NULL) const{
			if(ptm != NULL){
#ifdef SL_OS_WINDOWS
				gmtime_s(ptm, &m_time);
#else
				ptm = gmtime(&m_time);
#endif
				return ptm;
			}
			else{
				struct tm* ptmTemp;
#ifdef SL_OS_WINDOWS
				gmtime_s(ptmTemp, &m_time);
#else
				ptmTemp = gmtime(&m_time);
#endif
				return ptmTemp;
			}
		}

		struct tm* GetLocalTm(struct tm* ptm = NULL) const{
			if(ptm != NULL){
#ifdef SL_OS_WINDOWS
				localtime_s(ptm, &m_time);
#else
				ptm = localtime(&m_time);
#endif
				return ptm;
			}
			else{
				struct tm* ptmTemp;
#ifdef SL_OS_WINDOWS
				localtime_s(ptmTemp, &m_time);
#else
				ptmTemp = localtime(&m_time);
#endif
				return ptmTemp;
			}
		}

		static bool IsSameDay(time_t time1, time_t time2){
#ifdef SL_OS_WINDOWS
			struct tm t1, t2;
			localtime_s(&t1, &time1);
			localtime_s(&t2, &time2);
			return (t1.tm_year == t2.tm_year && t1.tm_mon == t2.tm_mon && t1.tm_mday == t2.tm_mday);
#else
			struct tm *t1, *t2;
			t1 = localtime(&time1);
			t2 = localtime(&time2);
			return (t1->tm_year == t2->tm_year && t1->tm_mon == t2->tm_mon && t1->tm_mday == t2->tm_mday);
#endif
		}

		static bool CompareTime(int time1, int time2, int iDay){
			time1 += iDay * 24 * 3600;
			time_t day1 = static_cast<time_t>(time1);
			time_t day2 = static_cast<time_t>(time2);
			struct tm *t1, *t2;
#ifdef SL_OS_WINDOWS
			struct tm tempT1, tempT2;
			localtime_s(&tempT1, &day1);
			localtime_s(&tempT2, &day2);
			t1 = &tempT1;
			t2 = &tempT2;
#else
			t1 = localtime(&day1);
			t2 = localtime(&day2);
#endif

			if (t1->tm_year > t2->tm_year){
				return true;
			}
			else if (t1->tm_year == t2->tm_year){
				if (t1->tm_mon > t2->tm_mon){
					return true;
				}
				else if (t1->tm_mon == t2->tm_mon){
					if (t1->tm_mday >= t2->tm_mday){
						return true;
					}
					else{
						return false;
					}
				}
				else{
					return false;
				}
			}
			else{
				return false;
			}
		}

		bool IsSameDay(time_t times){
			return IsSameDay(times, m_time);
		}

		bool IsSameDayWithMigration(time_t times, int ieff){
			if(times < 0 || ieff >= CTIME_SECONDS_PRE_DAY)
				return false;
			
			if(m_time - ieff < 0 || times - ieff < 0)
				return false;
			
			CTime tms(m_time - ieff);
			return tms.IsSameDay(times -ieff);
		}

		char* Format(char* pszBuffer, int iMaxLen, const char* pszFormat) const{
			time_t time = m_time;
			struct tm *ptmTemp = nullptr;
#ifdef SL_OS_WINDOWS
			struct tm ptm;
			localtime_s(&ptm, &time);
			ptmTemp = &ptm;
#else
			ptmTemp = localtime(&time);
#endif
			if(!strftime(pszBuffer, iMaxLen, pszFormat, ptmTemp)){
				pszBuffer[0] = '\0';
			}
			return pszBuffer;
		}
	private:
		time_t m_time;

	}; //class  CTime

	inline int64 getTimeMilliSecond(){
		return (int64)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	inline int64 getTimeNanoSecond(){
		return (int64)std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	inline const std::string getCurrentTimeStr(const char* format = "%4d-%02d-%02d %02d:%02d:%02d"){
		time_t tmp = time(nullptr);
		char info[128];
#ifdef SL_OS_WINDOWS
		tm t;
		localtime_s(&t, &tmp);
		SafeSprintf(info, sizeof(info), format, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
#else
		tm* t = localtime(&tmp);
		SafeSprintf(info, sizeof(info), format, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
#endif
		return info;
	}

	inline const std::string getTimeStr(const int64 tick, const char* format = "%4d-%02d-%02d %02d:%02d:%02d"){
		time_t tmp = (time_t)(tick / 1000);
		char info[128];
#ifdef SL_OS_WINDOWS
		tm t;
		localtime_s(&t, &tmp);
		SafeSprintf(info, sizeof(info), format, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
#else
		tm* t = localtime(&tmp);
		SafeSprintf(info, sizeof(info), format, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
#endif
		
		return info;
	}

}

#endif
