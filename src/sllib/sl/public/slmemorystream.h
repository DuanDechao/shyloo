/********************************************************************
	created:	2016/08/06
	created:	6:8:2016   15:43
	filename: 	c:\Users\ddc\Desktop\shyloo\libs\common\slmemorystream.h
	file path:	c:\Users\ddc\Desktop\shyloo\libs\common
	file base:	slmemorystream
	file ext:	h
	author:		ddc
	
	purpose:	用于网络数据流存储，或其他二进制流数据存储
*********************************************************************/

#ifndef _SL_MEMORYSTREAM_H_
#define _SL_MEMORYSTREAM_H_
#include "slobjectpool.h"
#include "slmemorystream_converter.h"
namespace sl
{
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

class MemoryStream
{

public:
	const static uint32 DEFAULT_SIZE = 0x100;
	MemoryStream():rpos_(0), wpos_(0)
	{
		data_.reserve(DEFAULT_SIZE);
	}

	MemoryStream(uint32 res):rpos_(0), wpos_(0)
	{
		if (res > 0)
			data_.reserve(res);
	}

	MemoryStream(const MemoryStream& buf)
		:rpos_(buf.rpos_), wpos_(buf.wpos_), data_(buf.data_)
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
		append((uint8*)&value, sizeof(value));
	}

	template<typename T> void put(int32 pos, T value)
	{
		EndianConvert(value);
		put(pos, (uint8*)&value, sizeof(value));
	}

	void swap(MemoryStream& s)
	{
		int32 rpos = s.rpos(), wpos = s.wpos();
		std::swap(data_, s.data_);
		s.rpos((int32)rpos_);
		s.wpos((int32)wpos_);

		rpos_ = rpos;
		wpos_ = wpos;
	}

	MemoryStream& operator<<(uint8 value)
	{
		append<uint8>(value);
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
		append((uint8 const *)value.c_str(), (int32)value.length());
		append((uint8)0);
		return *this;
	}

	MemoryStream& operator<<(const char* str)
	{
		append((uint8 const*)str, str ? (int32)strlen(str) : 0);
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

	uint8 operator[](int32 pos) const
	{
		return read<uint8>(pos);
	}

	int32 rpos() const {return rpos_;}

	int32 rpos(int32 rpos)
	{
		rpos_ = rpos < 0 ? 0 : rpos;
		return rpos_;
	}

	int32 wpos() const {return wpos_;}

	int32 wpos(int32 wpos)
	{
		wpos_ = wpos < 0 ? 0 : wpos;
		return wpos_;
	}

	template<typename T>
	void read_skip() { read_skip(sizeof(T)); }

	void read_skip(int32 skip)
	{
		if(skip > length()){
			SLASSERT(false, "read skip failed");
			return;
		}
		rpos_ += skip;
	}

	template<typename T> T read()
	{
		T r = read<T>(rpos_);
		rpos_ += sizeof(T);
		return r;
	}

	template<typename T> T read(int32 pos) const
	{
		SLASSERT(sizeof(T) <= length(), "read failed");

		T val = *((T const *)&data_[pos]);
		EndianConvert(val);
		return val;
	}

	void read(uint8* dest, int32 len)
	{
		if(len > length()){
			SLASSERT(false, "read failed");
			return;
		}

		memcpy(dest, &data_[rpos_], len);
		rpos_ += len;
	}

	uint8* data() {return &data_[0];}
	const uint8* data() const {return &data_[0];}

	//vector的大小
	virtual uint32 size() const {return (uint32)data_.size();}

	//vector是否为空
	virtual bool empty() const {return data_.empty();}

	//读索引到与写索引之间的长度
	virtual int32 length() const {return rpos() >= wpos() ? 0 : wpos() - rpos();}

	//剩余可填充的大小
	virtual int32 space() const {return wpos() >= (int32)size() ? 0 : (int32)size() - wpos();}

	//将读索引强制设置到写索引，表示操作结束
	void done() {read_skip(length());}

	void resize(uint32 newsize)
	{
		data_.resize(newsize);
		rpos_ = 0;
		wpos_ = size();
	}

	void data_resize(uint32 newsize)
	{
		data_.resize(newsize);
	}

	void reserve(uint32 ressize)
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
		append((uint8 const*)str.c_str(), (int32)str.size() + 1);
	}

	void append(const char* src, int32 cnt)
	{
		return append((const uint8*)src, cnt);
	}

	template<class T> void append(const T* src, uint32 cnt)
	{
		return append((const uint8*)src, cnt * sizeof(T));
	}

	void append(const uint8 *src, uint32 cnt)
	{
		if(!cnt) return;
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

	void insert(int32 pos, const uint8* src, uint32 cnt)
	{
		data_.insert(data_.begin() + pos, cnt, 0);
		memcpy(&data_[pos], src, cnt);
		wpos_ += cnt;

	}

	void put(int32 pos, const uint8* src, uint32 cnt)
	{
		if(pos + cnt > (int32)size()){
			SLASSERT(false, "put failed");
		}
		
		memcpy(&data_[pos], src, cnt);
	}

protected:
	mutable int32 rpos_, wpos_;
	std::vector<uint8> data_;
};

CREATE_OBJECT_POOL(MemoryStream);

}//namesapce sl
#endif