#ifndef SL_LOGIC_SLAVE_H
#define SL_LOGIC_SLAVE_H
#include "slikernel.h"
class Slave
{
public:
	int32 StartNode(sl::api::IKernel* pKernel, const char* cmd);

};

#endif