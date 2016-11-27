#include "slmemorystream.h"
namespace sl
{
static CObjectPool<MemoryStream> g_objPool("MemoryStream");

CObjectPool<MemoryStream>& MemoryStream::ObjPool()
{
	return g_objPool;
}

MemoryStream* MemoryStream::createPoolObject()
{
	return g_objPool.FetchObj();
}

void MemoryStream::reclaimPoolObject(MemoryStream* obj)
{
	g_objPool.ReleaseObj(obj);
}

void MemoryStream::destroyObjPool()
{
	ECHO("MemoryStream::destoryObjPool() size %d.\n", g_objPool.size());
	g_objPool.Destroy();
}

size_t MemoryStream::getPoolObjectBytes()
{
	size_t bytes = sizeof(rpos_) + sizeof(wpos_) + data_.capacity();
	return bytes;
}

void MemoryStream::onReclaimObject()
{
	if(data_.capacity() > DEFAULT_SIZE * 2)
		data_.reserve(DEFAULT_SIZE);

	clear(false);
}
}