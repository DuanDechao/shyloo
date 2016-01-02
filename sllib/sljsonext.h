#ifndef _LZ_JSON_EXT_H_
#define _LZ_JSON_EXT_H_
#include "slsize_string.h"
#include "slarray.h"
#include <json/json.h>
#include <lzbuffer.h>
#include <lzbitset.h>
#include <net/lzbase64.h>
#include <fstream>
#include <lzlog.h>
#include <lzutils.h>
#ifdef LZ_OS_LINUX
#include <ext/hash_map>
#include <lzhash_fun.h>
#else
#include <map>
#endif

using namespace std;

namespace lz
{
    enum EnumSetJsonErrorCode
    {
        //ERR_OK = 0,                       ///<�ɹ�
        ESJEC_OPEN_FILE_FAIL     = 100000,  ///<���ļ�ʧ��
        ESJEC_ARRAY_OUT_OF_RANGE = 100001,  ///<��������Խ�磬ʵ������ĸ�������Ԥ�������
        ESJEC_ARRAY2D_OUT_OF_ROW = 100002,  ///<������ά���飬����Խ��
        ESJEC_ARRAY2D_OUT_OF_COL = 100003,  ///<������ά���飬����Խ��
        ESJEC_JSON_PARSE_FAIL    = 100004,  ///<JSON����ʧ��
        ESJEC_UNKNOW_MAP_MODE    = 100005,  ///<δ֪�ĵ�ͼģʽ
        ESJEC_MAP_ID_ERROR       = 100006,  ///<����ĵ�ͼID
        ESJEC_UNKNOW_MAP_TYPE    = 100007,  ///<δ֪�ĵ�ͼ����
        ESJEC_MAP_ID_EXISTS      = 100008,  ///<Hash���У��Ѿ����ڸ�ID��map
        ESJEC_ALLOC_HASH_FAIL    = 100009,  ///<Hash�����ʧ��
    };

    #define SET_JSON_VALUE(Obj, JsonObject, Field) { int iRet = SetJsonValue(Obj.Field, JsonObject, #Field); if (!isOK(iRet)) return iRet; }
	#define SET_JSON_VALUE2(Field, JsonObject, FieldName) { int iRet = SetJsonValue(Field, JsonObject, #FieldName); if (!isOK(iRet)) return iRet; }
	#define SET_JSON_VALUE_ELEMECT(Obj, JsonObject) { int iRet = SetJsonValueNoName(Obj, JsonObject); if (!isOK(iRet)) return iRet; }

    //�����������ͺ�������
    //uint
    inline int SetJsonValue(unsigned int & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        a_StoreValue = a_Json[a_pszName].asUInt();
        return ERR_OK;
    }
    //bool
    inline int SetJsonValue(bool & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        a_StoreValue = a_Json[a_pszName].asBool();
        return ERR_OK;
    }
    //CSizeString
    template<int N>
    inline int SetJsonValue(CSizeString<N> & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        a_StoreValue = a_Json[a_pszName].asString().c_str();
        return ERR_OK;
    }
    //string
    inline int SetJsonValue(string & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        a_StoreValue = a_Json[a_pszName].asString().c_str();
        return ERR_OK;
    }
    //double
    inline int SetJsonValue(double & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        a_StoreValue = a_Json[a_pszName].asDouble();
        return ERR_OK;
    }
    //short
    inline int SetJsonValue(short & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        a_StoreValue = (short)(a_Json[a_pszName].asInt());
        return ERR_OK;
    }
    //ushort
    inline int SetJsonValue(unsigned short & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        a_StoreValue = (unsigned short)(a_Json[a_pszName].asUInt());
        return ERR_OK;
    }
    //int
    inline int SetJsonValue(int & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        a_StoreValue = a_Json[a_pszName].asInt();
        return ERR_OK;
    }
  //  //long long
  //  inline int SetJsonValue(long long & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
  //  {
		//// ���������⣬�˴�a_Json[a_pszName]����ת��String
  //      a_StoreValue = StringToLongDef(a_Json[a_pszName].asString().c_str());
  //      return ERR_OK;
  //  }

  //  //unsigned long long
  //  inline int SetJsonValue(unsigned long long & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
  //  {
  //      a_StoreValue = StringToDDWordDef(a_Json[a_pszName].asString().c_str());
  //      return ERR_OK;
  //  }


    //uint
    inline int SetJsonValueNoName(Json::UInt & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue = a_Json.asUInt();
        return ERR_OK;
    }
    //bool
    inline int SetJsonValueNoName(bool & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue = a_Json.asBool();
        return ERR_OK;
    }
    //CSizeString
    template<int N>
    inline int SetJsonValueNoName(CSizeString<N> & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue = a_Json.asString().c_str();
        return ERR_OK;
    }
    //string
    inline int SetJsonValueNoName(string & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue = a_Json.asString().c_str();
        return ERR_OK;
    }
    //double
    inline int SetJsonValueNoName(double & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue = a_Json.asDouble();
        return ERR_OK;
    }
    //short
    inline int SetJsonValueNoName(short & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue = (short)a_Json.asInt();
        return ERR_OK;
    }
    //ushort
    inline int SetJsonValueNoName(ushort & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue = (ushort)a_Json.asUInt();
        return ERR_OK;
    }
    //int
    inline int SetJsonValueNoName(int & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue = a_Json.asInt();
        return ERR_OK;
    }
    //long long
    inline int SetJsonValueNoName(long long & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue = StringToLongDef(a_Json.asString().c_str());
        return ERR_OK;
    }

    //unsigned long long
    inline int SetJsonValueNoName(unsigned long long & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue = StringToDDWordDef(a_Json.asString().c_str());
        return ERR_OK;
    }

    template<class T>
    inline int SetJsonValue(T & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        Json::Value & JsonList = a_Json[a_pszName];
        SET_JSON_VALUE_ELEMECT(a_StoreValue, JsonList);
        return ERR_OK;
    }

    //һά����
    template<class T, int MAX_COUNT>
    inline int SetJsonValue(CArray<T,MAX_COUNT> & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        a_StoreValue.Clear();
        Json::Value & JsonList = a_Json[a_pszName];
        Json::UInt uCount = JsonList.size();
        if (uCount > MAX_COUNT)
        {
            LZ_ERROR("SetJsonValue fail, conf count %d, value count %d", uCount, MAX_COUNT);
            return ESJEC_ARRAY_OUT_OF_RANGE;
        }
        for (Json::UInt i = 0; i < uCount; i++)
        {
            SET_JSON_VALUE_ELEMECT(*a_StoreValue.Append(), JsonList[i]);
        }
        return ERR_OK;
    }
    //
    template<class T, int MAX_COUNT>
    inline int SetJsonValueNoName(CArray<T,MAX_COUNT> & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue.Clear();
        Json::UInt uCount = a_Json.size();
        if (uCount > MAX_COUNT)
        {
            return ESJEC_ARRAY_OUT_OF_RANGE;
        }
        for (Json::UInt i = 0; i < uCount; i++)
        {
            SET_JSON_VALUE_ELEMECT(*a_StoreValue.Append(), a_Json[i]);
        }
        return ERR_OK;
    }
    //vector
    template<class T>
    inline int SetJsonValue(vector<T> & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        a_StoreValue.clear();
        Json::Value & JsonList = a_Json[a_pszName];
        Json::UInt uCount = JsonList.size();
        for (Json::UInt i = 0; i < uCount; i++)
        {
            T t;
            SET_JSON_VALUE_ELEMECT(t, JsonList[i]);
            a_StoreValue.push_back(t);
        }
        return ERR_OK;
    }
    template<class T>
    inline int SetJsonValueNoName(vector<T> & a_StoreValue, Json::Value & a_Json)
    {
        a_StoreValue.clear();
        Json::UInt uCount = a_Json.size();
        for (Json::UInt i = 0; i < uCount; i++)
        {
            T t;
            SET_JSON_VALUE_ELEMECT(t, a_Json[i]);
            a_StoreValue.push_back(t);
        }
        return ERR_OK;
    }
    //��ά����
    template<class T,int MAX_ROW, int MAX_COL>
    inline int SetJsonValue(lz::CArray< lz::CArray<T, MAX_COL>, MAX_ROW> & a_StoreValue, Json::Value & a_Json, const char * a_pszName)
    {
        a_StoreValue.Clear();
        Json::Value & JsonList = a_Json[a_pszName];
        Json::UInt uRowCount = JsonList.size();
        if (uRowCount > MAX_ROW)
        {
            return ESJEC_ARRAY2D_OUT_OF_ROW;
        }
        for (Json::UInt i = 0; i < uRowCount; i++)
        {
            Json::Value & JsonRow = JsonList[i];
            Json::UInt uColCount = JsonRow.size();
            if (uColCount > MAX_COL)
            {
                a_StoreValue.Clear();
                return ESJEC_ARRAY2D_OUT_OF_COL;
            }
            SET_JSON_VALUE_ELEMECT(*a_StoreValue.Append(), JsonRow);
        }
        return ERR_OK;
    }

    /**
    * json�е�����
    */
    template<typename T>
    class CJsonArray
    {
    public:
        vector<T> list;

        void Reset()
        {
            list.clear();
        }
    };

    template<typename T>
    inline int SetJsonValueNoName(CJsonArray<T> & a_Data, Json::Value & a_Json)
    {
        a_Data.Reset();
        SET_JSON_VALUE(a_Data, a_Json, list);
        return 0;
    }

    /**
    * װ�����ݵ�buffer��.
    * ������װ�ص�Json�ļ�ת���base64���룬�·���ȥ
    */
    inline int LoadFileToBuffer(CBuffer & a_Buffer, const char * a_pszFile)
    {
        a_Buffer.Detach();

        FILE * fp = fopen(a_pszFile, "rb");
        if (fp == NULL)
        {
            return ESJEC_OPEN_FILE_FAIL;
        }
        fseek(fp, 0, SEEK_END);
        int iFileSize = (int)ftell(fp); //�ļ���С
        int iBufferSize = iFileSize / 3 * 4 + 5; //ת��Base64��Ĵ�С
        
        char srcdata[4], destdata[5];
        srcdata[3] = 0;

        a_Buffer.Attach(NULL, iBufferSize, 0);
        fseek(fp, 0, SEEK_SET);
        while(!feof(fp))
        {
            size_t iReadSize = fread(srcdata, 1, 3, fp);
            if (iReadSize == 0) break;
            srcdata[iReadSize] = 0;
            base64::EncodeBase64(srcdata, (int)iReadSize, destdata);
            a_Buffer.Append(destdata, 4);
        }
        a_Buffer.AppendChar(0);
        fclose(fp);
        return ERR_OK;
    }

    // �����ͱ�����ԭ����ǰ������
    template<class T>
    inline int LoadJsonList(T& a_paramValue, Json::Value& paramJson)
    {
        SET_JSON_VALUE_ELEMECT(a_paramValue, paramJson);
        return 0;
    }

    /**
    * װ��Json����.
    *
    * @param [in] a_pszFileName Json���ļ���
    * @param [out] a_Data Ҫװ�ص�����
    * Json������base64����ķ�ʽװ��a_buffer��
    */
    template<class T>
    inline int LoadJson(const char * a_pszFileName, T & a_Data)
    {
        a_Data.Reset();
        ifstream r(a_pszFileName);
        if (!r)
        {
            return ESJEC_OPEN_FILE_FAIL;
        }
        Json::Reader JsonReader;
        Json::Value JsonRoot;
        if (!JsonReader.parse(r, JsonRoot))
        {
            LZ_ERROR("Parse Json (%s) Fail: %s", a_pszFileName, JsonReader.getFormatedErrorMessages().c_str());
            return ESJEC_JSON_PARSE_FAIL;
        }
        int iRet = LoadJsonList(a_Data, JsonRoot);
        r.close();        
        return iRet;
    }

    /**
    * װ��Json�����Json����a_Buffer��.
    *
    * @param [in] a_pszFileName Json���ļ���
    * @param [out] a_Data Ҫװ�ص�����
    * @param [out] a_Buffer Json���������
    * Json������base64����ķ�ʽװ��a_buffer��
    */
    template<class T>
    inline int LoadJson(const char * a_pszFileName, T & a_Data, CBuffer & a_Buffer)
    {
        int iRet = LoadJson(a_pszFileName, a_Data);
        if (!isOK(iRet))
        {
            return iRet;
        }
        iRet = LoadFileToBuffer(a_Buffer, a_pszFileName);
        return iRet;
    }
	//��ȡTXT�ļ�
// 	inline int ReadTxt(const char* PathName,char buff[])
// 	{
// 		FILE * fp = fopen(PathName, "r");
// 		if (fp == NULL)
// 		{
// 			return ESJEC_OPEN_FILE_FAIL;
// 		}
// 		int sizeFile = fseek(fp, 0, SEEK_END);
// 		fseek(fp, 0, SEEK_SET);
// 		fread(buff, 1, sizeFile, fp);
// 		fclose(fp);
// 		return 0
// 	}
    /**
    * json�е�hash��
    *
    * Ҫ��ʵ��T����ʵ��1��typedef��3������:
    * typedef xxx KEY;
    * bool IsCorrect()
    * bool IsValid()
    * const KEY& GetKey() �� KEY GetKey()
    */
    template<typename T>
    class CJsonHash
    {
    public:
#ifdef LZ_OS_LINUX
        typedef __gnu_cxx::hash_map<typename T::KEY, T> TJsonHash;
#else
        typedef std::map<typename T::KEY, T> TJsonHash;
#endif

    protected:
        TJsonHash m_stHash;
        typedef CJsonArray<T> TJsonArray;

    public:
        virtual ~CJsonHash() {}

        TJsonHash& GetHash()
        {
            return m_stHash;
        }

        /**
        * װ�����ã����ŵ�hash��
        */
        int Init(const char* pszJsonFile)
        {
            m_stHash.clear();

            //��ȡ����
            TJsonArray stArray;
            int iRet = LoadJson(pszJsonFile, stArray);
            if (iRet)
            {
                LZ_ERROR("load json file (%s) fail!", pszJsonFile);
                return iRet;
            }

            const int iCount = stArray.list.size();
            for (int i = 0; i < iCount; ++i)
            {
                T& st = stArray.list[i];
                if (!st.IsCorrect())
                {
                    LZ_ERROR("item is incorrect, index=%d", i);
                    return -1;
                }

                if (!st.IsValid())
                {
                    continue;
                }

                //���Ƿ��ظ�
                if (m_stHash.find(st.GetKey()) != m_stHash.end())
                {
                    LZ_ERROR("item is incorrect, index=%d", i);
                    return -1;
                }

                m_stHash.insert(pair<typename T::KEY, T>(st.GetKey(), st));
            }

            return AfterInit();
        }

        /** ���̳������������� */
        virtual int AfterInit()
        {
            return 0;
        }
    };

    /**
    * json�е����飬��Ҫ���±��GetKey()���ϸ��Ӧ��
    *
    * Ҫ��ʵ��T����ʵ��1��typedef��3������:
    * typedef xxx KEY;
    * bool IsCorrect()
    * bool IsValid()
    * const KEY& GetKey() �� KEY GetKey()
    *
    * COUNT ��ʾ������������
    * BASE ��ʾ�±��Ӧ����ʼֵ��0��ʾlist[0].GetKey() == 0, 1��ʾlist[0].GetKey() == 1
    * ʹ�÷����ɲο�CVipPrivilege
    */
    template<typename T, int COUNT, int BASE>
    class CJsonIndexArray
    {
    public:
        enum
        {
            INDEX_START = BASE,
        };

        typedef CArray<T, COUNT> TArray;

    protected:
        TArray  m_stArray;

    public:
        virtual ~CJsonIndexArray() {}

        TArray& GetArray()
        {
            return m_stArray;
        }

        /**
        * װ�����ã����ŵ�vector��
        */
        int Init(const char* pszJsonFile)
        {
            m_stArray.Clear();

            //��ȡ����
            CJsonArray<T> stArray;
            int iRet = LoadJson(pszJsonFile, stArray);
            if (iRet)
            {
                LZ_ERROR("load json file (%s) fail!", pszJsonFile);
                return iRet;
            }

            //���ϸ��Level���±��Ӧ

            CBitset<COUNT> stBitmap;   //������¼ĳ���±��Ƿ����ù�
            stBitmap.reset();

            //�ж������Ƿ����
            if ((int)stArray.list.size() > m_stArray.GetCount())
            {
                LZ_ERROR("%s items count(%d) > %d", pszJsonFile, stArray.list.size(), m_stArray.GetCount());
                return -1;
            }
            m_stArray.m_iUsedCount = stArray.list.size();

            for (int i = 0; i < (int)stArray.list.size(); ++i)
            {
                T& stTemp = stArray.list[i];
                if (!stTemp.IsCorrect())
                {
                    LZ_ERROR("item is incorrect, index=%d", i);
                    return -1;
                }

                if (!stTemp.IsValid())
                {
                    continue;
                }

                int iIndex = stTemp.GetKey() - INDEX_START;
                if (iIndex < 0 && iIndex >= m_stArray.m_iUsedCount) //�±곬����Χ
                {
                    LZ_ERROR("%s config invalid: key=%d i=%d, index invalid", pszJsonFile, stTemp.GetKey(), i);
                    return -1;
                }

                if (stBitmap.test(iIndex)) //˵���ظ���
                {
                    LZ_ERROR("%s config invalid: key=%d i=%d, repeat", pszJsonFile, stTemp.GetKey(), i);
                    return -1;
                }

                stBitmap.set(iIndex);
                m_stArray[iIndex] = stTemp;
            }

            const int i1count = stBitmap.count();
            if (i1count != m_stArray.m_iUsedCount)  //˵��������(1/2/3/5/6��ȱ��4)
            {
                LZ_ERROR("%s config invalid: lost(%d, %d)", pszJsonFile, i1count, m_stArray.m_iUsedCount);
                return -1;
            }

            return AfterInit();
        }

        /**
        * ����Key�������±꣬Ȼ����T
        */
        T* GetByKey(int iKey)
        {
            int iIndex = iKey - INDEX_START;
            if (iIndex < 0 && iIndex >= m_stArray.m_iUsedCount) //�±곬����Χ
            {
                return NULL;
            }

            return &(m_stArray[iIndex]);
        }

        /**
        * ����Key�������±꣬Ȼ����T
        */
        const T* GetByKey(int iKey) const
        {
            int iIndex = iKey - INDEX_START;
            if (iIndex < 0 && iIndex >= m_stArray.m_iUsedCount) //�±곬����Χ
            {
                return NULL;
            }

            return &(m_stArray[iIndex]);
        }

        /** ���̳������������� */
        virtual int AfterInit()
        {
            return 0;
        }
    };

    //-----------------------------------------------------

}
#endif
