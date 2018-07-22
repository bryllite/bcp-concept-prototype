#include "p2p_client.hpp"

p2p_client::p2p_client(boost::asio::io_service& io_service) : _io_service(io_service), _socket(io_service)
{
};


bool p2p_client::connected( void ) 
{
	return _connected.load() && _socket.is_open();
};

void p2p_client::onConnected( bool connected ) 
{
	log( "p2p_client::onConnected(%s)", connected ? "true" : "false" );
};

void p2p_client::onDisconnected( void ) 
{
	log( "p2p_client::onDisconnected()" );
};

void p2p_client::onReceived( message* msg ) 
{
	log( "p2p_client::onReceived()" );
};

void p2p_client::onWrited( size_t bytes_transferred ) 
{
//	log( "p2p_client::onWrited()" );
};

// connect
bool p2p_client::connect( void )
{
	if( connected() ) 
	{
		wlog( "tcp_client::connect() >> already connected" );
		return false;
	}

	_socket.async_connect( _remote_endpoint, boost::bind( &p2p_client::handle_connect, this, boost::asio::placeholders::error ) );
	return true;
};

bool p2p_client::connect( const char* host, unsigned short port ) 
{
	_remote_endpoint = tcp::endpoint( boost::asio::ip::address::from_string( host ), port );
	return connect();
};


// connect handler
void p2p_client::handle_connect( const boost::system::error_code& error )
{
	_connected.store( error == 0 );
	onConnected( ( _connected.load() ) );

	if( connected() )
		do_read();
};

// write message
bool p2p_client::write( message* msg )
{
	if( nullptr == msg ) return false;
	if( !msg->valid() ) return false;

	return write( ( const char* )msg, msg->size() );
};

// write data
bool p2p_client::write( const char* data, size_t len )
{
	_assert( data != nullptr && len <= max_buffer_size, "tcp_client::write() >> data=%x, len=%d", data, len );

	if( data == nullptr || len > max_buffer_size ) {
		elog( "tcp_client::write() : invalid message, data=%x, len=%d", data, len );
		return false;
	}

	if( !connected() ) {
		wlog( "tcp_client::write() : not connected" );
		return false;
	}

	// copy data to write buffer
	byte* buffer = _write_buffer.next();
	memcpy( buffer, data, len );

	// send request
	boost::asio::async_write( _socket, boost::asio::buffer( buffer, len ),
							  boost::asio::transfer_exactly( len ),
							  boost::bind( &p2p_client::handle_write, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );

	return true;
};

// write handler
void p2p_client::handle_write( const boost::system::error_code& error, size_t bytes_transferred )
{
	if( error ) 
	{
		wlog( "p2p_client::handle_write() >> error=%d", error );
		close();
		return;
	}

	onWrited( bytes_transferred );
};

// wait for receive completion
void p2p_client::do_read( void )
{
	if( connected() )
	{
		byte* buffer = _read_buffer.next();

		boost::asio::async_read( _socket, boost::asio::buffer( buffer, _read_buffer.length() ),
									boost::asio::transfer_exactly( sizeof( message ) ),
									boost::bind( &p2p_client::handle_read_header, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
	}
};

// read header handler 
void p2p_client::handle_read_header( const boost::system::error_code& error, size_t bytes_transferred )
{
	if( error ) {
		wlog( "p2p_client::handle_read_header() >> error: %s", error == boost::asio::error::eof ? "Connection lost" : error.message().c_str() );
		close();
		return;
	}

	if( bytes_transferred < sizeof( message ) ) {
		elog( "p2p_client::handle_read_header() >> bytes_transferred=%d", bytes_transferred );
		close();
		return;
	}

	byte* buffer = _read_buffer.get();

	size_t read_len = 0;
	message* header = reinterpret_cast<message*>( buffer );
	if( !header->valid() ) {
		elog( "p2p_client::handle_read_header() >> header validation failed" );
		close();
		return;
	}

	// ready payload
	boost::asio::async_read( _socket, boost::asio::buffer( buffer+bytes_transferred, _read_buffer.length() - bytes_transferred ), 
		boost::asio::transfer_exactly( header->payload_size() ),
		boost::bind( &p2p_client::handle_read_body, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, header ) );
};

// read body handler
void p2p_client::handle_read_body( const boost::system::error_code& error, size_t bytes_transferred, message* header )
{
	if( error )
	{
		wlog( "p2p_client::handle_read_body() >> error: %s", error == boost::asio::error::eof ? "Connection lost" : error.message().c_str() );
		close();
		return;
	}

	if ( header->size() > bytes_transferred + sizeof(message) )
	{
		wlog( "p2p_client::handle_read_body(): bytes_transferred=%d, header.size()=%d", bytes_transferred, header->size() );
		close();
		return ;
	}

//	message* msg = reinterpret_cast<message*>( _read_buffer.read_pos() );
	message* msg = header;
	if ( !msg->valid() || bytes_transferred + sizeof(message) < msg->size() )
	{
		elog( "p2p_client::handle_read_body(): message not valid" );
		close();
		return ;
	}

	// received callback with message copy ctor
	onReceived( msg );

	// wait for another read
	do_read();
};



// close connection
void p2p_client::close( void )
{
	// close socket
	if( _socket.is_open() ) 
	{
//		_socket.cancel();
		_socket.shutdown( tcp::socket::shutdown_both );
		_socket.close();
	}

	// disconnect callback
	if( _connected.load() ) 
	{
		onDisconnected();
		_connected.store( false );
	}
};
