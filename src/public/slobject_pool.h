#ifndef __SL_PUBLIC_OBJECT_POOL_H__
#define __SL_PUBLIC_OBJECT_POOL_H__
#include "slmulti_sys.h"
#include "slobject_allocator.h"
namespace sl{
template<typename _T, typename _Alloc = SLOjbectAllocator<_T>>
class SLOjbectPool{
	enum{
		POOL_INIT_SIZE = 100,
	};
public:
	SLOjbectPool(int32 initCount = POOL_INIT_SIZE)
		:_allocator(initCount)
	{}

	template<typename... Args>
	inline void init(Args... args){
		_allocator.init(args...);
	}

	template<typename... Args>
	inline _T* create(Args... args){
		return _allocator.allocate(args...);
	}

	inline void recover(_T * t){
		_allocator.deallocate(t);
	}

private:
	_Alloc	_allocator;
};
}
#endif