//#include <stdio.h>
//#include <string.h>
//#include "slsize_string.h"
//#include "slcode_convert.h"
//using namespace sl;
//
//class CDataTest: public CCodeObject
//{
//public:
//	int id;
//	short name;
//};
//BEGIN_CODE_CONVERT(CDataTest)
//CODE_CONVERT(id)
//CODE_CONVERT(name)
//END_CODE_CONVERT(CDataTest)
//
//int main()
//{
//	CDataTest pdata;
//	pdata.id = 102;
//	pdata.name = 50;
//
//	char* pszBuf = (char*)malloc(1024);
//	CCodeStream s(pszBuf, 1024);
//	CodeConvert(s, pdata, NULL, bin_encode());
//	
//	CDataTest deData;
//	s.InitConvert();
//	CodeConvert(s, deData, NULL, bin_decode());
//	printf("%d, %hd\n", deData.id, deData.name);
//
//
//
//}