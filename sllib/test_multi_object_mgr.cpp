//#include "slobject_mgr.h"
//#include "slmulti_object_mgr.h"
//using namespace sl;
//int main()
//{
//	const char* pszObjectConfig = "(2,4)(4,10)(8,10)(16,40)";
//	CMultiObjectMgr astObjectMgr;
//	int iBufferSize = CMultiObjectMgr::CountSize(pszObjectConfig, 2, 1);
//	char* pstBuffer = new char[iBufferSize];
//	astObjectMgr.Init(pszObjectConfig, pstBuffer, iBufferSize);
//	int mgrIndex = astObjectMgr.CalcMgrIndex(8);
//	CMgrIndex iMgrIndex;
//	char *p = astObjectMgr.Alloc(mgrIndex,&iMgrIndex);
//	printf("%d %d %p\n",iMgrIndex.iMgrIndex, iMgrIndex.iObjIndex, p);
//	CMgrIndex iMgrIndex1;
//	char *q = astObjectMgr.Alloc(mgrIndex, &iMgrIndex1);
//	printf("%d %d %p\n",iMgrIndex1.iMgrIndex, iMgrIndex1.iObjIndex, q);
//	
//
//}