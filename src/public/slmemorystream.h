/********************************************************************
	created:	2016/08/06
	created:	6:8:2016   15:43
	filename: 	c:\Users\ddc\Desktop\shyloo\libs\common\slmemorystream.h
	file path:	c:\Users\ddc\Desktop\shyloo\libs\common
	file base:	slmemorystream
	file ext:	h
	author:		ddc
	
	purpose:	
*********************************************************************/

#ifndef _SL_MEMORYSTREAM_H_
#define _SL_MEMORYSTREAM_H_
#include "slobjectpool.h"
#include "slmemorystream_converter.h"
namespace sl
{
class MemoryStreamException
{
public:
	MemoryStreamException(bool _add, size_t _pos, size_t _esize, size_t _size)
		:m_bAdd(_add),m_sPos(_pos),m_sESize(_esize),m_sSize(_size)
	{
		PrintPosError();
	}

	void PrintPosError() const
	{
		//SL_ERROR("Attempted to %s in MemoryStream (pos:%d size:%d).\n",
		//	(m_bAdd ? "put" : "get"), m_sPos, m_sSize);
	}
private:
	bool		m_bAdd;
	size_t		m_sPos;
	size_t      m_sESize;
	size_t		m_sSize;
};


/*
	将常用数据类型二进制序列化与反序列化

	使用方法：
			MemoryStream  stream;
			stream << (int64)1000000000;
			stream << (uint8)1;
			stream << "sl";
			stream.print_storage();
			uint8 n,n1;
			int64 x;
			std::string a;
			stream >> x;
			stream >> n;
			stream >> n1;
			stream >> a;
			printf("还原：%lld, %d, %d, %s", x, n, n1, a.c_str());
*/

class MemoryStream: public PoolObject
{
public:
	union PackFloatXType
	{
		float	fv;
		UINT32	uv;
		int		iv;
	};

	static CObjectPool<MemoryStream>& ObjPool();
	static MemoryStream* createPoolObject();
	static void reclaimPoolObject(MemoryStream* obj);
	static void destroyObjPool();
	virtual void onReclaimObject();
	virtual size_t getPoolObjectBytes();

public:
	const static size_t DEFAULT_SIZE = 0x100;
	MemoryStream():rpos_(0), wpos_(0)
	{
		data_.reserve(DEFAULT_SIZE);
	}

	MemoryStream(size_t res):rpos_(0), wpos_(0)
	{
		if (res > 0)
			data_.reserve(res);
	}

	MemoryStream(const MemoryStream& buf)
		:rpos_(buf.rpos_), wpos_(buf.wpos_),data_(buf.data_)
	{}

	virtual ~MemoryStream()
	{
		clear(true);
	}

	void clear(bool clearData)
	{
		if(clearData)
			data_.clear();

		rpos_ = wpos_ = 0;
	}

	template<typename T> void append(T value)
	{
		EndianConvert(value);
		append((byte*)&value, sizeof(value));
	}

	template<typename T> void put(size_t pos, T value)
	{
		EndianConvert(value);
		//put(pos)
		put(pos, (byte*)&value, sizeof(value));
	}

	void swap(MemoryStream& s)
	{
		size_t rpos = s.rpos(), wpos = s.wpos();
		std::swap(data_, s.data_);
		s.rpos((int)rpos_);
		s.wpos((int)wpos_);

		rpos_ = rpos;
		wpos_ = wpos;
	}

	MemoryStream& operator<<(byte value)
	{
		append<byte>(value);
		return *this;
	}

	MemoryStream& operator<<(uint16 value)
	{
		append<uint16>(value);
		return *this;
	}

	MemoryStream& operator<<(uint32 value)
	{
		append<uint32>(value);
		return *this;
	}

	MemoryStream& operator<<(uint64 value)
	{
		append<uint64>(value);
		return *this;
	}

	MemoryStream& operator<<(int8 value)
	{
		append<int8>(value);
		return *this;
	}

	MemoryStream& operator<<(int16 value)
	{
		append<int16>(value);
		return *this;
	}

	MemoryStream& operator<<(int32 value)
	{
		append<int32>(value);
		return *this;
	}

	MemoryStream& operator<<(int64 value)
	{
		append<int64>(value);
		return *this;
	}

	MemoryStream& operator<<(float value)
	{
		append<float>(value);
		return *this;
	}

	MemoryStream& operator<<(double value)
	{
		append<double>(value);
		return *this;
	}

	MemoryStream& operator<<(const std::string& value)
	{
		append((uint8 const *)value.c_str(), value.length());
		append((uint8)0);
		return *this;
	}

	MemoryStream& operator<<(const char* str)
	{
		append((uint8 const*)str, str ? strlen(str) : 0);
		append((uint8)0);
		return *this;
	}

	MemoryStream& operator<<(bool value)
	{
		append<int8>(value);
		return *this;
	}

	MemoryStream& operator>>(bool& value)
	{
		value = read<char>() > 0 ? true: false;
		return *this;
	}

	MemoryStream& operator>>(uint8& value)
	{
		value = read<uint8>();
		return *this;
	}

	MemoryStream& operator>>(uint16& value)
	{
		value = read<uint16>();
		return *this;
	}

	MemoryStream& operator>>(uint32& value)
	{
		value = read<uint32>();
		return *this;
	}

	MemoryStream& operator>>(uint64& value)
	{
		value = read<uint64>();
		return *this;
	}

	MemoryStream& operator>>(int8& value)
	{
		value = read<int8>();
		return *this;
	}
	MemoryStream& operator>>(int16& value)
	{
		value = read<int16>();
		return *this;
	}
	MemoryStream& operator>>(int32& value)
	{
		value = read<int32>();
		return *this;
	}
	MemoryStream& operator>>(int64& value)
	{
		value = read<int64>();
		return *this;
	}
	MemoryStream& operator>>(float& value)
	{
		value = read<float>();
		return *this;
	}

	MemoryStream& operator>>(double& value)
	{
		value = read<double>();
		return *this;
	}

	MemoryStream& operator>>(std::string& value)
	{
		value.clear();
		while(length() > 0)
		{
			char c = read<char>();
			if(c == 0 || !isascii(c))
				break;

			value += c;
		}

		return *this;
	}

	MemoryStream& operator>>(char* value)
	{
		while(length() > 0)
		{
			char c = read<char>();
			if(c == 0 || !isascii(c))
				break;
			*(value++) = c;
		}
		*value = '\0';
		return *this;
	}

	uint8 operator[](size_t pos) const
	{
		return read<uint8>(pos);
	}





	size_t rpos() const {return rpos_;}

	size_t rpos(int32 rpos)
	{
		if(rpos < 0)
			rpos = 0;
		
		rpos_ = rpos;
		return rpos_;
	}

	size_t wpos() const {return wpos_;}

	size_t wpos(int32 wpos)
	{
		if(wpos < 0)
			wpos = 0;
		wpos_ = wpos;
		return wpos_;
	}

	template<typename T>
	void read_skip() {read_skip(sizeof(T));}

	void read_skip(size_t skip)
	{
		if(skip > length())
			throw MemoryStreamException(false, rpos_, skip, length());

		rpos_ += skip;
			
	}

	template<typename T> T read()
	{
		T r = read<T>(rpos_);
		rpos_ += sizeof(T);
		return r;
	}

	template<typename T> T read(size_t pos) const
	{
		if(sizeof(T) > length())
			throw MemoryStreamException(false, pos, sizeof(T), length());

		T val = *((T const *)&data_[pos]);
		EndianConvert(val);
		return val;
	}

	void read(uint8* dest, size_t len)
	{
		if(len > length())
			throw MemoryStreamException(false, rpos_, len, length());

		memcpy(dest, &data_[rpos_], len);
		rpos_ += len;
	}






	uint8* data() {return &data_[0];}
	const uint8* data() const {return &data_[0];}

	//vector的大小
	virtual size_t size() const {return data_.size();}

	//vector是否为空
	virtual bool empty() const {return data_.empty();}

	//读索引到与写索引之间的长度
	virtual size_t length() const {return rpos() >= wpos() ? 0 : wpos() - rpos();}

	//剩余可填充的大小
	virtual size_t space() const {return wpos() >= size() ? 0 : size() - wpos();}

	//将读索引强制设置到写索引，表示操作结束
	void done() {read_skip(length());}

	void resize(size_t newsize)
	{
		data_.resize(newsize);
		rpos_ = 0;
		wpos_ = size();
	}

	void data_resize(size_t newsize)
	{
		data_.resize(newsize);
	}

	void reserve(size_t ressize)
	{
		if(ressize > size())
			data_.reserve(ressize);
	}

	void appendBlob(const char* src, uint32 cnt)
	{
		(*this) << cnt;

		if(cnt > 0)
			append(src, cnt);

	}

	void appendBlob(const std::string& datas)
	{
		uint32 len = (uint32)datas.size();
		(*this) << len;

		if(len > 0)
			append(datas.data(), len);
	}

	void appendBlob(const MemoryStream* stream)
	{
		uint32 len = (uint32)stream->length();
		(*this) << len;
		
		if(len > 0)
			append(*stream);
	}

	void append(const std::string& str)
	{
		append((uint8 const*)str.c_str(), str.size() + 1);
	}

	void append(const char* src, size_t cnt)
	{
		return append((const uint8*)src, cnt);
	}

	template<class T> void append(const T* src, size_t cnt)
	{
		return append((const uint8*)src, cnt * sizeof(T));
	}

	void append(const uint8 *src, size_t cnt)
	{
		if(!cnt)
			return;
		SLASSERT(size() < 10000000, "wtf");

		if(data_.size() < wpos_ + cnt)
			data_.resize(wpos_ + cnt);

		memcpy(&data_[wpos_], src, cnt);
		wpos_ += cnt;
	}

	void append(const MemoryStream& buffer)
	{
		if(buffer.wpos())
		{
			append(buffer.data() + buffer.rpos(), buffer.length());
		}
	}

	/*void appendPackY(float y)
	{
	PackFloatXType yPackData;
	yPackData.fv = y;

	yPackData.fv += yPackData.iv < 0 ? -2.f : 2.f;
	uint16 data = 0;
	data = (yPackData.uv >> 12) 
	}*/

	void insert(size_t pos, const byte* src, size_t cnt)
	{
		data_.insert(data_.begin() + pos, cnt, 0);
		memcpy(&data_[pos], src, cnt);
		wpos_ += cnt;

	}

	void put(size_t pos, const byte* src, size_t cnt)
	{
		if(pos + cnt > size())
			throw MemoryStreamException(true, pos, cnt, size());
		
		memcpy(&data_[pos], src, cnt);
	}

protected:
	mutable size_t rpos_, wpos_;
	std::vector<uint8> data_;
};


}//namesapce sl
#endif