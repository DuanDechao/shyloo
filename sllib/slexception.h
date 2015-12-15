///�쳣��
/********************************************************************
	created:	2015/12/15
	created:	15:12:2015   16:24
	filename: 	d:\workspace\shyloo\sllib\slexception.h
	file path:	d:\workspace\shyloo\sllib
	file base:	slexception
	file ext:	h
	author:		ddc
	
	purpose:	�쳣��
*********************************************************************/

#ifndef _SL_EXCEPTION_H_
#define _SL_EXCEPTION_H_

#include <lzbase_define.h>
#include <lzstring_utils.h>
#include <lzlog.h>

#ifndef SL_OS_WINDOWS
#include <execinfo.h>
#endif

#ifdef SL_NO_EXCEPTION
#define SL_THROW(...)
#else
#define SL_THROW(...) throw EException(__VA_ARGS__)
#endif

namespace lz
{
    ///�쳣����
    class EException
    {
        public:
            /**
            * ����/����
            */
            EException(int iErrorCode = -1) 
                :m_iErrorCode(iErrorCode)
            {
                DumpErrorBackTrace();
            }

            EException(const char* pszFormat, ...) 
                :m_iErrorCode(-1)
            {
                va_list ap;
                va_start(ap, pszFormat);
                CStringUtils::FormatV(m_strErrorMsg, pszFormat, ap);
                va_end(ap);

                DumpErrorBackTrace();
            }

            EException(int iErrorCode, const char* pszFormat, ...)
                :m_iErrorCode(iErrorCode)
            {

                va_list ap;
                va_start(ap, pszFormat);
                CStringUtils::FormatV(m_strErrorMsg, pszFormat, ap);
                va_end(ap);

                DumpErrorBackTrace();
            }

            virtual ~EException()
            {
            }

            /**
            * Getϵ��
            */
            int GetErrorCode() const
            {
                return m_iErrorCode;
            }

            const char* GetErrorMsg() const
            {
                return m_strErrorMsg.c_str();
            }

            const string & GetErrorString() const
            {
                return m_strErrorMsg;
            }

            const char* what() const
            {
                return m_strErrorMsg.c_str();
            }
        protected:
            //add by rex 2011-3-21
            string & GetErrorString()
            {
                return m_strErrorMsg;
            }
            void setErrorCode(int a_iErrorCode)
            {
                m_iErrorCode = a_iErrorCode;
            }
            //add end

            void DumpErrorBackTrace()
            {
                SL_ERROR("Exception: ErrorCode=%d Msg=%s", m_iErrorCode, m_strErrorMsg.c_str());
                DumpBackTrace();
            }

    public:
            static void DumpBackTrace()
            {
                // ����ʱ�������-rdynamic����, backtrace_symbolsֻ�᷵�ص�ַ��Ϣ
                // ����ͨ��addr2line���߲鵽��ַ��Ӧ���ļ��к�
                if (!(SL_ELOG->GetFilter() & EFatal))
                {
                    return;
                }

#ifndef LZ_OS_WINDOWS
                void* array[20];
                int iCount = backtrace(array, LZ_COUNTOF(array));
                char** symbols = backtrace_symbols(array, iCount);
                CAutoFree autop(symbols);
                string sBackTrace = "StackTrace:\n";
                for (int i = 1; i < iCount; ++i)  // ��1����DumpBackTrace, ����
                {
                    sBackTrace += "    ";
                    sBackTrace += symbols[i];
                    sBackTrace += "\n";
                }

                SL_FATAL("%s", sBackTrace.c_str());
#endif
            }

        protected:
            int         m_iErrorCode;
            string      m_strErrorMsg;
    };
}
#endif
