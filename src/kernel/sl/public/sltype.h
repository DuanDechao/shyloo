//����shyloo�Ļ�����������
#ifndef _SL_TYPE_H_
#define _SL_TYPE_H_

#include <string>
#include <cstdio>
#include <cstdlib>
#include <IPTypes.h>
#include <IPHlpApi.h>
namespace sl
{
//�޷�������
typedef unsigned char		uint8;          ///< �ֽ�����   8λ�޷�������
typedef char				int8;			///< �ֽ�����   8λ��������

typedef unsigned short		uint16;         ///< ������     16λ�޷�������
typedef short				int16;          ///< ������     16λ��������

typedef unsigned int        uint32;         ///< ˫������   32λ�޷�������
typedef int					int32;          ///< ˫������   32λ��������

typedef unsigned long long  uint64;         ///< ��������   64λ�޷�������
//�з�������
typedef signed long long    int64;          ///< 64λ�з�������
	
typedef wchar_t				wchar;          ///< �����
typedef void*				pointer;        ///< ������ָ��
using std::string;                          ///< ����STL��String����

typedef int64				datetime_t;     ///< 64λ��ʱ������
typedef uint64				uid_t;          ///< �û�ID����


typedef int32				ENTITY_ID;		///< entityID������
typedef uint64				COMPONENT_ID;	///< һ�������������id
}// namespace sl
#endif