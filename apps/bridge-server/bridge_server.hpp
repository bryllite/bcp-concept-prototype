#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>


// bridge server
class bridge_server : public bryllite::net::tcp_server, public bryllite::net::tcp_client
{
	using tcp = boost::asio::ip::tcp;
	using tcp_server = bryllite::net::tcp_server;
	using tcp_session = bryllite::net::tcp_session;

protected:
	// game server session
	class game_session : public tcp_session
	{
	public:
		game_session( boost::asio::io_service& ios ) : tcp_session( ios ) {
		};
	};

public:
	bridge_server() {
	};

	// start & stop server
	bool start_server( const char* node_host, unsigned short node_port, unsigned short game_port );
	bool stop_server( void );

protected:
	// tcp_session -> game_session allocator
	tcp_session* new_session( void );

protected:
	void onConnected( bool connected );
	void onDisconnected( void );
	void onWrited( size_t bytes_transferred );
	void onReceived( message* msg );

	// bridge <-> game server message handler
	void onTcpMessage( tcp_session* session, message* msg );
	void onGameMessagePing( game_session* session, message_ping* msg );
	void onGameMessagePong( game_session* session, message_pong* msg );

	// node <-> bridge server message handler
	void onNodeMessagePing( message_ping* msg );
	void onNodeMessagePong( message_pong* msg );
	void onNodeMessageTravel( message_travel* msg );
	void onNodeMessagePop( node_bridge_message_pop* msg );

};
