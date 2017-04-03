#ifndef __SL_PUBLIC_POOL_H__
#define __SL_PUBLIC_POOL_H__
#include "slmulti_sys.h"
#include "sltools.h"
namespace sl{

template<typename T, int32 chunkCount = 1, int32 chunkSize = 64>
class SLPool{
	enum{
		IN_FREE = 0,
		IN_USE,
	};

	struct ChunkList;
	struct Chunk{
		char _buffer[sizeof(T)];
		ChunkList* _parent;
		Chunk* _prev;
		Chunk* _next;
		int8   _state;
		int32  _len;
#ifdef _DEBUG
		const char _file[SL_PATH_MAX];
		int32 _line;
#endif // _DEBUG

	};

	struct ChunkList{
		ChunkList*	_prev;
		ChunkList*	_next;
		int32		_useCount;
		Chunk		_chunk[chunkSize];
	};

public:
	SLPool() :_head(nullptr), _listHead(nullptr), _chunkCount(0){
		allocChunk(1);
	}

	~SLPool(){
		while (_listHead){
			ChunkList* next = _listHead->_next;
			SLFREE(_listHead);
			_listHead = next;
		}
	}

	inline int32 count() { return _chunkCount * chunkSize; }

	template<typename... Args>
	T* create(const char* file, int32 line, Args... args){
		Chunk* newChunk = alloc(file, line);
		SLASSERT(newChunk, "create chunk failed");
		return new(newChunk->_buffer)T(args...);
	}

	T* create(const char* file, int32 line){
		Chunk* newChunk = alloc(file, line);
		SLASSERT(newChunk, "create chunk failed");
		return new(newChunk->_buffer)T();
	}

	void recover(T* obj){
		obj->~T();
		recover((Chunk*)obj);
	}

private:
	Chunk* alloc(const char* file, int32 line){
		if (!_head)
			allocChunk(1);

		SLASSERT(_head, "create chunk failed");

		Chunk* ret = _head;
		remove(_head);
		SLASSERT(ret->_state == IN_FREE && ret->_len == sizeof(Chunk), "buffer is not in free");

		ret->_state = IN_USE;
		++ret->_parent->_useCount;
#ifdef _DEBUG
		sl::SafeMemcpy((void*)ret->_file, sizeof(ret->_file), file, strlen(file));
		ret->_line = line;
#endif

		return ret;
	}

	void recover(Chunk* chunk){
		SLASSERT(chunk->_len == sizeof(Chunk) && chunk->_state == IN_USE, "invaild object memory or not in use");
		SLASSERT(chunk->_parent->_useCount > 0, "chunk is not in list");

		sl::SafeMemset(chunk->_buffer, sizeof(chunk->_buffer), 0, sizeof(chunk->_buffer));
		chunk->_state = IN_FREE;
		chunk->_parent->_useCount--;

		if (chunk->_parent->_useCount == 0 && _chunkCount > chunkCount)
			freeChunkList(chunk->_parent);
		else
			add(chunk);
	}

	void allocChunk(int32 count){
		for (int32 i = 0; i < count; i++){
			ChunkList* newChunk = (ChunkList*)SLMALLOC(sizeof(ChunkList));
			SLASSERT(newChunk, "create chunk failed");
			sl::SafeMemset(newChunk, sizeof(ChunkList), 0, sizeof(ChunkList));
			arrangeChunkList(newChunk);

			newChunk->_next = _listHead;
			if (_listHead)
				_listHead->_prev = newChunk;
			_listHead = newChunk;
		}

		_chunkCount += count;
	}

	void arrangeChunkList(ChunkList* chunkList){
		chunkList->_prev = nullptr;
		chunkList->_next = nullptr;
		chunkList->_useCount = 0;

		for (int32 i = 0; i < chunkSize; i++){
			chunkList->_chunk[i]._prev = nullptr;
			chunkList->_chunk[i]._next = nullptr;
			chunkList->_chunk[i]._state = IN_FREE;
			chunkList->_chunk[i]._len = sizeof(Chunk);
			chunkList->_chunk[i]._parent = chunkList;
			add(&(chunkList->_chunk[i]));
		}
	}

	void freeChunkList(ChunkList* chunkList){
		for (int32 i = 0; i < chunkSize; i++){
			SLASSERT(chunkList->_chunk[i]._state == IN_FREE, "not free chunk");
			remove(&(chunkList->_chunk[i]));
		}

		if (chunkList->_prev)
			chunkList->_prev->_next = chunkList->_next;

		if (chunkList->_next)
			chunkList->_next->_prev = chunkList->_prev;

		if (chunkList == _listHead)
			_listHead = chunkList;

		chunkList->_prev = nullptr;
		chunkList->_next = nullptr;
		SLFREE(chunkList);

		_chunkCount--;
	}

	void add(Chunk* chunk){
		chunk->_next = _head;
		if (_head)
			_head->_prev = chunk;

		_head = chunk;
	}

	void remove(Chunk* chunk){
		if (chunk->_next)
			chunk->_next->_prev = chunk->_prev;

		if (chunk->_prev)
			chunk->_prev->_next = chunk->_next;

		if (chunk == _head)
			_head = chunk->_next;

		chunk->_prev = nullptr;
		chunk->_next = nullptr;
	}

private:
	Chunk*		_head;
	ChunkList*	_listHead;
	int32		_chunkCount;
};

}
#ifdef SL_OS_WINDOWS
#define CREATE_FROM_POOL(pool, ...) pool.create(__FILE__, __LINE__, ##__VA_ARGS__)
#else
#define CREATE_FROM_POOL(pool, a...) pool.create(__FILE__, __LINE__, ##a)
#endif

#endif