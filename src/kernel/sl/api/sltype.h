#ifndef SL_SLTYPE_H
#define SL_SLTYPE_H
#include <functional>
#define SLAPI __stdcall

typedef short				MessageID;

typedef std::function<void(const char* pBuf, int len)> msgHandlerCB;
#endif