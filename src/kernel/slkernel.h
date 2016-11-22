#ifndef SL_KERNEL_H
#define SL_KERNEL_H
#include "slikernel.h"
#include "slsingleton.h"
namespace sl
{
namespace core
{
class Kernel: public api::IKernel, public CSingleton<Kernel>
{

};
}
}
#endif