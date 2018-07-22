#include "p2p_session.hpp"

p2p_session::p2p_session( boost::asio::io_service& ios ) 
	: _ios(ios), _socket( ios ), _active( false ), _closed( 0 )
{
};

p2p_session::~p2p_session() 
{
};

bool p2p_session::active( void ) 
{ 
	return _active.load(); 
};

void p2p_session::active( bool active_ ) 
{ 
	_active.store( active_ ); 
};

boost::asio::ip::tcp::socket& p2p_session::socket( void ) 
{ 
	return _socket; 
};

void p2p_session::close( void )
{
	_closed = bryllite::timestamp();
	active( false );

	if( _socket.is_open() )
	{
		_socket.shutdown( tcp::socket::shutdown_both );
		_socket.close();
	}
};

bool p2p_session::expired( time_t timeout ) 
{
	return !active() && ( _closed != 0 && bryllite::timestamp() - _closed > timeout );
};

byte* p2p_session::read_buffer(void) 
{
	return _read_buffer.get();
};

byte* p2p_session::next_read_buffer( void )
{
	return _read_buffer.next();
}

byte* p2p_session::write_buffer(void)
{
	return _write_buffer.get();
};

byte* p2p_session::next_write_buffer( void )
{
	return _write_buffer.next();
};

size_t p2p_session::read_buffer_length( void )
{
	return _read_buffer.length();
};

size_t p2p_session::write_buffer_length( void )
{
	return _write_buffer.length();
};

bool p2p_session::write( message* msg )
{
	return write( (byte*)msg, msg->size() );
};


bool p2p_session::write( byte* data, size_t len )
{
	_assert( data != nullptr && len <= write_buffer_length(), "tcp_server::write() >> data=%x, len=%d", data, len );

	if( data == nullptr || len > write_buffer_length() ) 
	{
		elog( "p2p_session::write() : invalid message, data=%x, len=%d", data, len );
		return false;
	}

	// copy data to write buffer
	byte* buffer = _write_buffer.get();
	memcpy( buffer, data, len );
	_write_buffer.next();

	// async write to socket
	boost::asio::async_write( socket(), boost::asio::buffer( buffer, write_buffer_length() ), 
		boost::asio::transfer_exactly( len ), 
		boost::bind( &p2p_session::handle_write, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );

	return true;
};

void p2p_session::handle_write( const boost::system::error_code& error, size_t bytes_transferred )
{
//	_write_buffer.next();
};
