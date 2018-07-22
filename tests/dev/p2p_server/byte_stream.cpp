#include "byte_stream.hpp"

byte_stream::byte_stream() : _stream_buffer(nullptr), _buffer_size(0), _read_pos(0), _write_pos(0)
{
};

byte_stream::byte_stream(size_t length ) : byte_stream()
{
	prepare(length);
};

byte_stream::~byte_stream()
{
	cleanup();
};

void byte_stream::cleanup( void )
{
	smartlock(this);

	if ( _stream_buffer != nullptr )
	{
		delete[] _stream_buffer;
		_stream_buffer = nullptr;
	}

	_buffer_size = _read_pos = _write_pos = 0;
};

// is allocated?
bool byte_stream::allocated( void )
{
	return ( _stream_buffer != nullptr && _buffer_size > 0 );
};

// buffer size
size_t byte_stream::size( void )
{
	return _buffer_size;
};

// data length
size_t byte_stream::length( void )
{
	return _write_pos>_read_pos?_write_pos-_read_pos:0;
};

// prepare buffer
void byte_stream::prepare( size_t size )
{
	cleanup();

	if ( size > 0 )
	{
		smartlock(this);

		_stream_buffer = new byte[ size + 1 ];
		_buffer_size = size ;
	}
};

// resize buffer
void byte_stream::resize( size_t size )
{
	if ( size == 0 || _buffer_size == size ) return ;

	if ( allocated() ) 
	{
		smartlock(this);

		// allocate new buffer
		byte* new_buffer = new byte[ size + 1 ];
		size_t len = length(), copy_len = 0;

		// copy old buffer to new buffer
		if ( len > 0 )
		{
			if ( size < len ) 
				wlog( "byte_stream::resize(): buffer data truncated! length=%d, resize=%d", len, size );

			copy_len = std::min< size_t >( len, size );
			memcpy( new_buffer, _stream_buffer+_read_pos, copy_len );
		} 

		_buffer_size = size;
		_read_pos = 0;
		_write_pos = copy_len;

		// remove old buffer & set new buffer
		delete[] _stream_buffer;
		_stream_buffer = new_buffer;
	}
	else 
	{
		prepare( size );
	}
};

// clear stream buffer
void byte_stream::clear( void )
{
	_read_pos = _write_pos = 0;
};

// get free space
size_t byte_stream::free_space( void )
{
	return _buffer_size - _write_pos;
};


byte* byte_stream::read_pos( void ) 
{
	return _stream_buffer + _read_pos ;
};

byte* byte_stream::write_pos( void )
{
	return _stream_buffer + _write_pos ;
};

byte* byte_stream::peek( size_t& len )
{
	len = length();
	return read_pos();
};


// write data to buffer
size_t byte_stream::write( byte* data, size_t len )
{
	smartlock(this);

	if ( _write_pos + len > size() )
		resize( _write_pos + len + 1 );

	memcpy( write_pos(), data, len );
	write_commit(len);

	return len;
};

// read data from buffer
size_t byte_stream::read( byte* data, size_t len )
{
	smartlock(this);

	if ( length() == 0 ) return 0;

	size_t copy_len = std::min< size_t >( length(), len );
	memcpy( data, read_pos(), copy_len );

	read_commit( copy_len );
	return copy_len;
};


void byte_stream::read_commit( size_t bytes_read )
{
	smartlock(this);

	_read_pos += bytes_read;

	if ( _read_pos * 2 > size() && length() == 0 ) 
//	if ( length() == 0 ) 
	{
		dlog( "byte_stream cleared" );
		clear();
	}
};

void byte_stream::write_commit( size_t bytes_write )
{
	smartlock(this);

	_write_pos += bytes_write;
};

byte_stream& byte_stream::operator<<(int& value )
{
	write( (byte*)&value, sizeof( value ) );
	return *this;
};

byte_stream& byte_stream::operator<<(std::string& value )
{
	write( (byte*)value.c_str(), value.length() );
	return *this;
};

byte_stream& byte_stream::operator>>(int& value )
{
	read( (byte*)&value, sizeof(value) );
	return *this;
};
