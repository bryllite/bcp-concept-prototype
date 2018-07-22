#pragma once


// socket buffer : byte[COUNT+1][LENGTH+1]
template <size_t COUNT, size_t LENGTH >
class socket_buffer
{
protected:
	byte _data[ COUNT + 1 ][ LENGTH + 1 ];
	size_t _current_pos;

public:
	socket_buffer() : _current_pos(0)
	{
	};

	byte* get( void )
	{
		return ( _current_pos < COUNT ) ? _data[ _current_pos ] : nullptr ;
	};

	byte* get( size_t pos )
	{
		return ( pos < COUNT ) ? _data[ pos ] : nullptr;
	};
	
	byte* next( void )
	{
		size_t pos;
		return next(pos);
	};

	byte* next( size_t& pos )
	{
		_current_pos = ++_current_pos % COUNT;
		pos = _current_pos;
		return get();
	};

	size_t current_pos( void )
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
