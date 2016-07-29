//����shyloo�Ļ�����������
#ifndef _SL_TYPE_H_
#define _SL_TYPE_H_

#include <string>
#include <cstdio>
#include <cstdlib>

namespace sl
{
	//�޷�������
	typedef unsigned char		byte;           ///< �ֽ�����   8λ�޷�������

	typedef unsigned short		ushort;         ///< ������     16λ�޷�������
	typedef unsigned int        uint;           ///< ˫������   32λ�޷�������
	typedef unsigned long long  uint64;         ///< ��������   64λ�޷�������

	//�з�������
	typedef signed long long    int64;          ///< 64λ�з�������
	
	typedef wchar_t				wchar;          ///< �����
	typedef void*				pointer;        ///< ������ָ��
	using std::string;                          ///< ����STL��String����

	typedef int64				datetime_t;     ///< 64λ��ʱ������
	typedef uint64				uid_t;          ///< �û�ID����


}// namespace sl
#endif