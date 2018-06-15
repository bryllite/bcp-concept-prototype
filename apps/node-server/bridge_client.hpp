#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

#include "../bridge_message.inl"




// bridge client
class bridge_client : public bryllite::net::tcp_client
{
	using message_header = bryllite::net::message_header;
	using timer = bryllite::timer;

protected:
	std::unique_ptr< std::thread > _io_thread;

public:
	bridge_client( boost::asio::io_service& ios );
	bridge_client( boost::asio::io_service& ios, const char* host, unsigned short port );

	void start( const char* host, unsigned short port );
	void stop( void );

protected:
	void onConnected( bool connected );
	void onDisconnected( void );
	void onWrited( size_t bytes_transferred );
	void onReceived( message_header* msg );

	void onPing( bridge_node_message_ping* msg );
	void onPong( bridge_node_message_pong* msg );
};

