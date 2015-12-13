///�����ļ���
///�Ӷ�������ļ���ȡ���õ���������
/********************************************************************
	created:	2015/12/06
	created:	6:12:2015   18:13
	filename: 	e:\workspace\shyloo\sllib\slini_config.h
	file path:	e:\workspace\shyloo\sllib
	file base:	slini_config
	file ext:	h
	author:		ddc
	
	purpose:	�����ļ���
				�Ӷ�������ļ���ȡ���õ���������
*********************************************************************/

#ifndef _SL_INI_CONFIG_H_
#define	_SL_INI_CONFIG_H_

///�����ļ���ʽ����(sSection��name���ִ�Сд)
// [sSection]
// name=value
// #ע��

#include <iostream>
#include <fstream>
#include <string>
#include "slbase.h"
#include <map>
#include "slstring_utils.h"
using namespace std;

namespace sl
{
	/// CConfig��ķ���ֵ����
	typedef enum enConfigRet
	{
		ECR_OK					=	0,				
		ECR_FILE_OPENFAIL,							///< 1:���ļ�ʧ��n
		ECR_SECTION_NOTEXIST,						///< 2:û�ҵ���Ӧ��sSectio
		ECR_KEY_NOTEXIST,							///< 3:û�ҵ���Ӧ��key
		ECR_PARSELINE_FAIL,							///< 4:��ȡ����ʱ������ʧ��
	} ECONFIG_RET;

	class CConfig
	{
	
	public:
		typedef map<string, string>			MapSection;
		typedef map<string, MapSection>		MapFile;
		typedef map<string, MapFile>		MapFiles;
	public:
		//piRet �������ض����õĽ��
		const string GetString(const char* pszFile, const char* pszSection,
			const char* pszKey, int* piRet = NULL)
		{
			string s(pszFile);

			//�����ļ�
			MapFiles::iterator itFiles = m_conf.find(s);
			if(itFiles == m_conf.end())
			{
				int iRet = LoadConfig(pszFile);
				if(iRet)
				{
					SetResult(piRet, iRet);
					return m_NULLString;
				}
				itFiles = m_conf.find(s);
			}

			//���ҽ�
			s = pszSection;
			CStringUtils::MakeLower(s);
			MapFile::iterator itFile = itFiles->second.find(s);
			if(itFile == itFiles->second.end())
			{
				SetResult(piRet, ECR_SECTION_NOTEXIST);
				return m_NULLString;
			}

			//������
			s = pszKey;
			CStringUtils::MakeLower(s);
			MapSection::iterator itSection = itFile->second.find(s);
			if(itSection == itFile->second.end())
			{
				SetResult(piRet, ECR_KEY_NOTEXIST);
				return m_NULLString;
			}

			SetResult(piRet, ECR_OK);
			return itSection->second;
			
		}

		/*
			ȡ�ַ������͵�����
			@param [out] piRet �������ض����õĽ��
			@note �������ֻ������ֻ��һ�������ļ�������Ҫ�ȵ���LoadConfig�������
		*/
		const string GetString(const char* pszSection, const char* pszKey, int* piRet = NULL)
		{
			if(m_conf.size() != 1)
			{
				SetResult(piRet, ECR_FILE_OPENFAIL);
				return m_NULLString;
			}

			MapFile mapFile = m_conf.begin()->second; //ֻ��һ�������ļ�

			string s = pszSection;
			CStringUtils::MakeLower(s);
			MapFile::iterator itFile = mapFile.find(s);
			if(itFile == mapFile.end())
			{
				SetResult(piRet, ECR_SECTION_NOTEXIST);
				return m_NULLString;
			}

			//������
			s = pszKey;
			CStringUtils::MakeLower(s);
			MapSection::iterator itSection = itFile->second.find(s);
			if(itSection == itFile->second.end())
			{
				SetResult(piRet, ECR_KEY_NOTEXIST);
				return m_NULLString;
			}

			SetResult(piRet, ECR_OK);
			return itSection->second;
		}
		
		//ȡuint��������
		/*
			@param [in]	a_File		ָ���������ļ�
			@param [in]	a_Section	ָ����Section
			@param [in]	a_Key		ָ����Key
			@param [out] a_Ret		�������ض����õĽ��
		*/
		unsigned int GetUInt(const char* a_File, const char* a_Section, const char* a_Key, int* a_Ret = NULL)
		{
			unsigned int uRet = 0;
			int iTempRet = ECR_OK;
			do 
			{
				string strValue = GetString(a_File, a_Section, a_Key, &iTempRet);
				if(iTempRet != ECR_OK)
				{
					break;
				}

				uRet = CStringUtils::StrToInt<unsigned int>(strValue.c_str(), -1, &iTempRet);
				if(iTempRet != 0)
				{
					break;
				}
			} while (false);
			if(a_Ret != NULL)
			{
				*a_Ret = iTempRet;
			}
			return uRet;
		}
		
		///ȡuint��������
		/*
			@param [in] a_Section ָ����Section
			@param [in] a_key     ָ����Key
			@param [out] a_Ret    �������ض����õĽ��
			@note
		*/
		unsigned int GetUInt(const char* a_Section, const char* a_Key, int* a_Ret = NULL)
		{
			unsigned int uRet = 0;
			int iTempRet = ECR_OK;
			do 
			{
				string strValue = GetString(a_Section, a_Key, &iTempRet);
				if(iTempRet != ECR_OK)
				{
					break;
				}

				uRet = CStringUtils::StrToInt<unsigned int>(strValue.c_str(), -1, &iTempRet);
				if(iTempRet != 0)
				{
					break;
				}
			} while (false);
			if(a_Ret != NULL)
			{
				*a_Ret = iTempRet;
			}
			return uRet;
		}

		int GetInt(const char* pszFile, const char* pszSection, const char* pszKey, int* piRet = NULL)
		{
			return atoi(GetString(pszFile, pszSection, pszKey, piRet).c_str());
		}
		
		/*
			ȡint���͵�����
			@param [out] piRet �������ض����õĽ��
			@note �������ֻ������ֻ��һ�������ļ�������Ҫ�ȵ���LoadConfig�������
		*/
		int GetInt(const char* pszSection, const char* pszKey, int* piRet = NULL)
		{
			return atoi(GetString(pszSection, pszKey, piRet).c_str());
		}



		//�ض�ĳ������
		int ReloadConfig(const char* pszFile)
		{
			MapFiles::iterator itFiles = m_conf.find(pszFile);
			if(itFiles != m_conf.end())
			{
				m_conf.erase(itFiles);
			}

			return LoadConfig(pszFile);
		}

		// ����Ѿ���������������
		void Clear()
		{
			m_conf.clear();
		}

		//��������
		int LoadConfig(const char* pszFile)
		{
			ifstream fin(pszFile);
			if(!fin.is_open())
			{
				return ECR_FILE_OPENFAIL;
			}

			MapFiles::iterator itFiles = m_conf.find(pszFile);
			if(itFiles != m_conf.end())
			{
				///�����ļ��Ѿ�����
				return ECR_OK;
			}

			MapFile empty;
			m_conf.insert(MapFiles::value_type(pszFile, empty));
			MapFile& mapFile = m_conf[pszFile];

			int line_count = 0;
			string inbuf, subbuf;
			string sSection = "";
			int iRet = 0;

			while(!fin.eof())
			{
				//��ȡһ������
				getline(fin, inbuf, '\n');
				++line_count;

				if(inbuf.empty())
				{
					continue;
				}

				//ini�����ļ���֧��unicode����֧��ansi��utf8
				//����Ǵ�BOM��Ϣ��utf8����Ҫȥ��BOM��Ϣ������ȷ����
				size_t begin_pos = 0;
				if(line_count == 1 && inbuf.length() > 2 &&
					inbuf[0] == '\xEF' && inbuf[1] == '\xBB' && inbuf[2] == '\xBF')
				{
					begin_pos = 3;
				}

				size_t last_pos = inbuf.find_last_of("\r\n");
				if(last_pos != string::npos)
				{
					subbuf = inbuf.substr(begin_pos, last_pos - begin_pos);
				}
				else
				{
					subbuf = inbuf.substr(begin_pos);
				}

				//����һ������
				iRet = ParseLine(mapFile, sSection, subbuf);
				if(iRet)
				{
					return iRet;
				}

			}

			return ECR_OK;

		}
	protected:

		int ParseLine(MapFile& mapFile, string& sSection, string& line)
		{
			size_t first = 0, last = 0;

			Trim(line, "\t\r\n");

			//�����Ƿ���ע���У���#Ϊע��
			first = line.find_first_of("#");
			if(first != string::npos)
			{
				line = line.substr(0, first);
			}
			if(line.empty())
			{
				return ECR_OK;
			}

			//�����Ƿ�ΪsSection
			last = line.rfind(']');
			if(line[0] == '[' && last != string::npos)
			{
				//�µ�һ��
				sSection = line.substr(1, last-1);
				CStringUtils::MakeLower(sSection);
				MapSection empty;
				mapFile.insert(MapFile::value_type(sSection, empty));
				return ECR_OK;
			}

			//Ϊһ����������
			if(sSection.empty())
			{
				return ECR_PARSELINE_FAIL;
			}
			first = line.find('=');
			if(first == string::npos)
			{
				//û�� =
				return ECR_PARSELINE_FAIL;
			}

			string name = line.substr(0, first);
			string value = line.substr(first + 1, string::npos);

			Trim(name, " \t\r\n");
			Trim(value, " \t\r\n");
			CStringUtils::MakeLower(name);
			if(name.empty())
			{
				return ECR_PARSELINE_FAIL;
			}

			MapSection& mapSection = mapFile[sSection];
			mapSection.insert(MapSection::value_type(name, value));
			return ECR_OK;


		}

		string& Trim(string& line, const char* trim)
		{
			size_t first = line.find_first_not_of(trim);
			size_t last  = line.find_last_not_of(trim);
			if(first == string::npos || last == string::npos)
			{
				line = "";
			}
			else
			{
				line = line.substr(first, last-first+1);
			}
			return line;
		}
		int SetResult(int *piRet, int iRet)
		{
			if(piRet)
			{
				*piRet = iRet;
			}

			return iRet;
		}
	private:
		MapFiles		m_conf;
		string			m_NULLString;   ///< ���ַ���

	};

}
#endif