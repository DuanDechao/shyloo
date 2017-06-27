//循环buffer
#ifndef _SL_PUBLIC_RING_BUFFER_H_
#define _SL_PUBLIC_RING_BUFFER_H_
#include "sltime.h"
namespace sl{
	/*如果这个queue只有两个对象单工使用，那么可以不用互斥*/
class SLRingBuffer{
private:
	enum { SPACE = 8 };

public:
	SLRingBuffer() {}
	~SLRingBuffer() {}

	//初始化
	inline int32 init(int32 iLen, bool clear){
		if (clear){
			_offset		= sizeof(SLRingBuffer);
			_size		= iLen - sizeof(SLRingBuffer);
			_read		= 0;
			_write		= 0;
			_createTime = (uint32)sl::getTimeMilliSecond();
			_checkSum	= (uint32)_offset ^ (uint32)_size ^ (uint32)_createTime;
		}
		return 0;
	}

	inline char* writePtr(int32& size){
		if (_read > _write)
			size = _read - _write - SPACE;
		else
			size = (_read < SPACE ? (_size + _read - SPACE) : _size) - _read;

		return getBuffer() + _write;
	}
	
	inline int32 getFreeSize() const{ return _read > _write ? (_read - _write - SPACE) : (_size - _write + _read - SPACE); }
	inline int32 getDataSize() const { return _read > _write ? (_size - _read + _write) : (_write - _read); }

	inline int32 put(const char* pszBuffer, int32 iLen){
		if (iLen == 0){
			return 0;
		}
		
		//每次放入的长度最大不能超过0xFFFFFF  = 16M
		if (iLen < 0 || iLen >(_size - SPACE) || iLen > 0xFFFFFF){
			ECHO_ERROR("put buf to queue failed! len = %d, left len = %d", iLen, _size - SPACE);
			SLASSERT(false, "wtf");
			return -1;
		}

		//newpos表示完成Put后write的位置
		//wrap表示是否换行了
		const int32 startpos = _write;
		const int32 minpos = (_read < SPACE ? _size : 0) + _read - SPACE;
		int32 newpos = startpos + iLen;
		int32 wrap = 0;
		if (newpos >= _size){
			newpos -= _size;
			wrap = 1;
		}

		//判断是否会满
		if (_write > minpos){
			if (wrap && newpos > minpos){
				return -2;
			}
		}
		else{
			if (wrap || newpos > minpos){
				return -2;
			}
		}

		if (wrap){
			//buf起始位置已经超过了size_
			memcpy(getBuffer() + startpos, pszBuffer, _size - startpos);
			memcpy(getBuffer(), pszBuffer + _size - startpos, iLen - _size + startpos);
		}
		else{
			memcpy(getBuffer() + startpos, pszBuffer, iLen);
		}

		//调整write_指针
		_write = newpos;

		return 0;
	}

	inline const char* get(char* pszBuffer, int32 readLen, bool isPeek = false){
		if (readLen <= 0 || _read == _write){
			return nullptr;
		}

		const char* retBuf = nullptr;
		const int32 startpos = _read;
		if (startpos + readLen > _size){
			memcpy(pszBuffer, getBuffer() + startpos, _size - startpos);
			memcpy(pszBuffer + _size - startpos, getBuffer(), readLen - _size + startpos);
			retBuf = pszBuffer;
		}
		else{
			retBuf = getBuffer() + startpos;
		}

		if (!isPeek)
			_read = (startpos + readLen) % _size;

		return retBuf;
	}

	inline void readOut(const int32 len){
		const int startpos = _read;
		_read = (startpos + len) % _size;
	}

	inline void writeIn(const int32 len){
		const int startpos = _write;
		_write = (startpos + len) % _size;
	}

	//用于在创建时做校验
	//@return 0表示buffer对象已创建， 否则表示对象未创建
	inline int32 checkSum() const{
		if (!_offset || !_size || !_createTime){
			return -1;
		}
		uint32 nTempInt = (uint32)_offset ^ (uint32)_size ^ (uint32)_createTime ^ (uint32)_checkSum;
		return (nTempInt == 0) ? 0 : -1;
	}

private:
	inline char* getBuffer() const { return (char*)this + _offset; }
	inline int32 getSize() const { return _size; }

private:
	int32			_offset;
	int32			_size;
	int32			_read;		//已用的数据开始
	int32			_write;		//已用数据的结束


	//////////////////////
	uint32			_createTime; //创建时间
	uint32			_checkSum;	  //校验结果

};  

} //namespace sl

#endif