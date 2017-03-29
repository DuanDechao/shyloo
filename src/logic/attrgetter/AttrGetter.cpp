#define ATTR_EXPORT
#include "AttrGetter.h"
#include "Attr.h"

bool AttrGetter::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	return true;
}

bool AttrGetter::launched(sl::api::IKernel * pKernel){
	return true;
}

bool AttrGetter::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}
