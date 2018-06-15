#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>


// game server class
class game_server : public bryllite::net::tcp_server, public bryllite::net::tcp_client
{
	using tcp = boost::asio::ip::tcp;
	using tcp_server = bryllite::net::tcp_server;
	using tcp_session = bryllite::net::tcp_session;

	// user session
	class user_session : public tcp_session
	{
	protected:
		std::string _nickname;
		time_t _heartbeat;

	public:
		user_session( boost::asio::io_service& ios ) : tcp_session( ios ), _heartbeat( 0 ) {
		};
	};

public:
	game_server() {
	};

	// start & stop server
	bool start_server( const char* bridge_host, unsigned short bridge_port, unsigned short game_port );
	bool stop_server( void );

protected:
	// game server <-> game user message handler
	void onTcpMessage( tcp_session* session, message* msg );
	void onUserMessagePing( user_session* session, message_ping* msg );
	void onUserMessagePong( user_session* session, message_pong* msg );

	// bridge server <-> game server message handler
	void onConnected( bool connected );
	void onDisconnected( void );
	void onWrited( size_t bytes_transferred );
	void onReceived( message* msg );

	void onBridgeMessagePing( message_ping* msg );
	void onBridgeMessagePong( message_pong* msg );
	void onBridgeMessageTravel( message_travel* msg );
	void onBridgeMessagePop( bridge_game_message_pop* msg );
};
