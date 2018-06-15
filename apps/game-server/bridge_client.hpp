#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

#include "../bridge_message.inl"

// message map for bridge message
#define		begin_bridge_msg_map( _msg )			bridge_message* __message__ = _msg; switch( _msg->msgid() ) {
#define		entry_bridge_msg_map( _msg, _proc )		case _msg##_id: { _proc( (_msg*)__message__ ); break ;	}
#define		end_bridge_msg_map()					default: break; }

// game server app
class game_server_app;

// bridge client
class bridge_client : public bryllite::net::tcp_client
{
	using message_header = bryllite::net::message_header;
	using timer = bryllite::timer;

protected:
	std::unique_ptr< std::thread > _io_thread;
	game_server_app& _server_app;

public:
	bridge_client( boost::asio::io_service& ios, const char* host, unsigned short port, game_server_app& app );

	void start( const char* host, unsigned short port );
	void stop( void );

protected:
	void onConnected( bool connected );
	void onDisconnected( void );
	void onWrited( size_t bytes_transferred );
	void onReceived( message_header* msg );

	void onPing( bridge_game_message_ping* msg );
	void onPong( bridge_game_message_pong* msg );
	void onTest( bridge_game_message_test* msg );
};

