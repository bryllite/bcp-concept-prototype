#pragma once

// byte stream class
class CByteStream
{
protected:
	typedef std::vector<byte> byte_vector;
	byte_vector _vch;
	size_t _read_pos;

public:
	typedef byte_vector::value_type value_type;
	typedef byte_vector::iterator iterator;
	typedef byte_vector::const_iterator const_iterator;
	typedef byte_vector::reverse_iterator reverse_iterator;

	CByteStream();
	CByteStream(const char* data, size_t size);
	CByteStream(const_iterator pbegin, const_iterator pend);
	CByteStream(const char* pbegin, const char* pend);
	CByteStream(const CByteStream& vchIn);

	const char operator[](size_t at) const;

	size_t size(void) const;
	bool empty(void) const;
	void clear(void);
	void compact(void);

	const_iterator begin(void) const;
	iterator begin(void);

	const_iterator end(void) const;
	iterator end(void);

	const value_type* data(void) const;
	value_type* data(void);

	// read from stream for nSize
	size_t read(char* pch, size_t nSize);

	// write stream in the end
	size_t write(const char* pch, size_t nSize);

	// byte stream to hex code
	const std::string toHex(void) const;

	// hex code to byte stream
	bool setHex(const std::string& hex);
};

