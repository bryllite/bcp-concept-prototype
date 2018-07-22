#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>


// byte stream class
class byte_stream : bryllite::lockable
{
public:
	byte* _stream_buffer;
	size_t _buffer_size;
	size_t _read_pos, _write_pos;

public:
	byte_stream();
	byte_stream(size_t length );
	virtual ~byte_stream();

	void cleanup( void );

	// is allocated?
	bool allocated( void );

	// buffer size
	size_t size( void );

	// data length
	size_t length( void );

	// prepare buffer
	void prepare( size_t size );

	// resize buffer
	void resize( size_t size );

	// clear stream buffer
	void clear( void );

	// get free space
	size_t free_space( void );

	byte* read_pos( void );
	byte* write_pos( void );

	byte* peek( size_t& len );

	// read/write data to/from buffer
	size_t write( byte* data, size_t len );
	size_t read( byte* data, size_t len );

	// read/write commit
	void read_commit( size_t bytes_read );
	void write_commit( size_t bytes_write );

	byte_stream& operator<<(int& value );
	byte_stream& operator<<(std::string& value );
	byte_stream& operator>>(int& value );

};


template <size_t COUNT, size_t LENGTH >
class session_buffer
{
protected:
	byte _data[ COUNT + 1 ][ LENGTH + 1 ];
	size_t _current_pos;

public:
	session_buffer(): _current_pos(0)
	{
	};

	byte* get( void )
	{
		return ( _current_pos < COUNT ) ? _data[_current_pos] : nullptr;
	};

	byte* next( void )
	{
		_current_pos = ++_current_pos % COUNT;
		return get();
	};

	size_t pos( void )
	{
		return _current_pos;
	};

	size_t count( void )
	{
		return COUNT;
	};

	size_t length( void )
	{
		return LENGTH;
	};
};



// network buffer 
class byte_buffer 
{
	enum { max_buffer_cnt = 16 };

protected:
	byte _data[ max_buffer_cnt + 1 ][ max_message_length + 1 ];
	size_t _idx;

public:
	byte_buffer() : _idx(0)
	{
	};

	byte* get( size_t idx )
	{
		if ( idx >= max_buffer_cnt ) return nullptr;
		return _data[idx];
	};

	byte* get( void )
	{
		if ( _idx >= max_buffer_cnt ) return nullptr;
		return _data[_idx];
	};

	byte* next( void )
	{
		_idx = ( ++_idx ) % max_buffer_cnt ;
		return get();
	};

	size_t idx( void ) 
	{
		return _idx;
	};
};
