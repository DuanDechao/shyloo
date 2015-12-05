#include "slbase_define.h"
#include "slcode_queue.h"
using namespace sl;
int main()
{
	SL_NLOG->AddFormat(EFileLine);
	SL_TRACE("log file write %d ok!", 7);
	char pst[20] = "hello world";
	CCodeQueue *pbuffer = (CCodeQueue*)new char[1600];
	pbuffer->Init(1024);
	pbuffer->Put(pst,12);
	char result[46] = {0};
	int ilen = 0;
	pbuffer->Get(result,46, ilen);
	printf("%s\n", result);
	delete [] pbuffer;
	return 0;
}