#ifndef __SL_PUBLIC_POOL_H__
#define __SL_PUBLIC_POOL_H__
#include "slmulti_sys.h"
template<typename T, int32 chunkCount = 1, int32 chunkSize = 64>
class SLPool{
	enum{
		IN_FREE = 0,
		IN_USE,
	};

	struct Chunk{
		char buffer[sizeof(T)];
		ChunkList* parent;
		Chunk* _prev;
		Chunk* _next;
		int8   _state;
		int32  _len;
	};

	struct ChunkList{
		ChunkList* _prev;
		ChunkList* _next;
		int32		_useCount;
		Chunk		_chunk[chunkSize];
	};
public:

	void recover(Chunk* chunk){
		SLASSERT(chunk->_len == sizeof(Chunk) && chunk->_state == IN_USE, "invaild object memory or not in use");
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
	chunk*		_head;
	chunkList*	_listHead;
	int32		_chunkCount;
};
#endif