#pragma once

/*----------------------
	Buffer Reader
----------------------*/
class BufferReader
{
public:
	BufferReader();
	BufferReader(unsigned char* buffer, unsigned __int32 size, unsigned __int32 pos = 0);
	~BufferReader();

	unsigned char* Buffer() { return _buffer; }
	unsigned __int32 Size() { return _size; }
	unsigned __int32 ReadSize() { return _pos; }
	unsigned __int32 FreeSize() { return _size - _pos; }

	template<typename T>
	bool Peek(T* dest) { return Peek(dest, sizeof(T)); }
	bool Peek(void* dest, unsigned __int32 len);

	template<typename T>
	bool Read(T* dest) { return Read(dest, sizeof(T)); }
	bool Read(void* dest, unsigned __int32 len);

	template<typename T>
	BufferReader& operator>>(OUT T& dest);

private:
	unsigned char* _buffer = nullptr;
	unsigned __int32 _size = 0;
	unsigned __int32 _pos = 0;
};

template<typename T>
inline BufferReader& BufferReader::operator>>(OUT T& dest)
{
	dest = *reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	
	return *this;
}
