#pragma once

// account address class
class CAddress
{
public:
	enum { min_length = 32, max_length = 35 };

protected:
	byte _data[max_length+1];

public:
	CAddress();
	CAddress( std::string address );

	void setNull(void);
	bool isNull(void) const;
	explicit operator bool() const;

	bool empty( void ) const;
	size_t length( void ) const;
	std::string str( void ) const;
	void str( std::string s );

	operator const char*() const;
	operator std::string() const;

	bool operator<( const CAddress& other ) const;
	bool operator<=( const CAddress& other ) const;
	bool operator>( const CAddress& other ) const;
	bool operator>=( const CAddress& other ) const;
	bool operator==( const CAddress& other ) const;
	bool operator!=( const CAddress& other ) const;
};
