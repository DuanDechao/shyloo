//�ļ��� �����ļ��Ķ�ȡ��д��

#ifndef _SL_FILE_MGR_H_
#define _SL_FILE_MGR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "slfile.h"
#include "sltype.h"
#include "slfile_base.h"
namespace sl
{
namespace file
{
class CSLFile :public ISLFile ,CReadFile, CWriteFile{
public:
	~CSLFile();
	virtual bool SLAPI close(void);
	virtual bool SLAPI flush(void);
	virtual bool SLAPI isOpened(void);
	virtual const char* SLAPI getFileName() const;

	//��ȡһ�У� ��delim��β
	virtual const char* SLAPI read(char delim);
	virtual const char* SLAPI readLine();
	virtual bool SLAPI writeLine(const char* pszBuf, int32 iLen);

private:
	FILE* m_pszFile; // �ļ�����ָ��
};

}
}// namespace sl
#endif