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
	OMemory(int32 size) : m_pszBuf(NULL), m_size(size){
		SLASSERT(m_size > 0, "invalid params");
		m_pszBuf = NEW char[m_size];
		sl::SafeMemset(m_pszBuf, m_size, 0, size);
	}

	~OMemory(){
		DEL[] m_pszBuf;
	}

	inline void setData(const MemLayout* layout, const void* data, const int32 size){
		SLASSERT(layout->_offset + size <= m_size, "out of range");
		SLASSERT(size <= layout->_size, "invalid data size");
		sl::SafeMemcpy(m_pszBuf + layout->_offset, layout->_size, data, size);
	}

	inline const void* getData(const MemLayout* layout){
		SLASSERT(layout->_offset + layout->_size <= m_size, "out of range");
		return m_pszBuf + layout->_offset;
	}

	inline void clear(){
		sl::SafeMemset(m_pszBuf, m_size, 0, m_size);
	}
private:
	char* m_pszBuf;
	const int32 m_size;
};
#endif