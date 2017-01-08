#ifndef _SL_FILE_H_
#define _SL_FILE_H_
#include "sltype.h"
namespace sl
{
namespace file
{
class ISLFile{
public:
	virtual bool SLAPI close(void) = 0;
	virtual bool SLAPI flush(void) = 0;
	virtual const char* SLAPI getFileName() const = 0;

	virtual const char* SLAPI readLine() = 0;
	virtual bool SLAPI writeLine(const char* pszBuf, int32 iLen) = 0;

};

const ISLFile* SLAPI openFile(const char* pszFileName);

}
}

#endif