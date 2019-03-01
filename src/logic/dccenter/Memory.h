#ifndef __SL_DC_MEMORY_H__
#define __SL_DC_MEMORY_H__
#include "sltype.h"
#include "sltools.h"
struct MemLayout{
	int32 _offset;
	int32 _size;
};

class OMemory{
public:
	OMemory(int32 size) : m_pszBuf(NULL), m_size(size), m_needFree(true){
		SLASSERT(m_size > 0, "invalid params");
		m_pszBuf = NEW char[m_size];
		sl::SafeMemset(m_pszBuf, m_size, 0, size);
	}

	OMemory(char* buf, const int32 bufSize): m_pszBuf(buf), m_size(bufSize), m_needFree(false){}

	~OMemory(){
		if(m_needFree){
			DEL[] m_pszBuf;
		}

		m_pszBuf = NULL;
	}

	inline char* buf() {return m_pszBuf;}
	inline const int32 size() {return m_size;}

	inline void setData(const MemLayout* layout, const void* data, const int32 size){
		SLASSERT(layout->_offset + size <= m_size, "out of range");
		SLASSERT(size <= layout->_size, "invalid data size");
		sl::SafeMemcpy(m_pszBuf + layout->_offset, layout->_size, data, size);
	}

	inline void setData(const int32 offset, const MemLayout* layout, const void* data, const int32 size){
		SLASSERT(offset + layout->_offset + size <= m_size, "out of range");
		SLASSERT(size <= layout->_size, "invalid data size");
		sl::SafeMemcpy(m_pszBuf + offset + layout->_offset, layout->_size, data, size);
	}

	inline const void* getData(const MemLayout* layout){
		SLASSERT(layout->_offset + layout->_size <= m_size, "out of range");
		return m_pszBuf + layout->_offset;
	}

	inline const void* getData(const int32 offset, const MemLayout* layout){
		SLASSERT(offset + layout->_offset + layout->_size <= m_size, "out of range");
		return m_pszBuf + offset + layout->_offset;
	}

	inline void clear(){
		sl::SafeMemset(m_pszBuf, m_size, 0, m_size);
	}

private:
	char* m_pszBuf;
	const int32 m_size;
	bool m_needFree;
};
#endif
