#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

#include "byte_stream.hpp"


// p2p client
class p2p_client
{
	using tcp = boost::asio::ip::tcp;
	enum { max_buffer_size = max_message_length };

protected:
	boost::asio::io_service& _io_service;
	tcp::socket _socket;
	tcp::endpoint _remote_endpoint;
	std::atomic<bool> _connected;

	session_buffer<16, max_buffer_size > _read_buffer;
	session_buffer<8, max_buffer_size > _write_buffer;

public:
	p2p_client(boost::asio::io_service& io_service);

	// get socket
	tcp::socket& socket( void ) {
		return _socket;
	};

	// connect
	bool connect( const char* host, unsigned short port );

	// close connection
	void close( void ); 

	// is connected?
	bool connected( void );

	// write data
	bool write( const char* data, size_t len );

	// write message
	bool write( message* msg );

protected:
	bool connect( void );


protected:
	virtual void onConnected( bool connected );
	virtual void onDisconnected( void );
	virtual void onReceived( message* msg );
	virtual void onWrited( size_t bytes_transferred );

private:
	// wait for receive completion
	void do_read( void );

	// connect handler
	void handle_connect( const boost::system::error_code& error );

	// write handler
	void handle_write( const boost::system::error_code& error, size_t bytes_transferred );

	// read header handler 
	void handle_read_header( const boost::system::error_code& error, size_t bytes_transferred );

	// read body handler
	void handle_read_body( const boost::system::error_code& error, size_t bytes_transferred, message* header );

};
