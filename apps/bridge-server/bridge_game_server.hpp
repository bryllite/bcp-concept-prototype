#pragma once

#include "../bridge_message.inl"

// bridge-game message handler
#define		begin_game_msg_map( _session, _msg )	game_session* __game_session__=_session; bridge_message* __message__=_msg; switch( _msg->msgid() ) {
#define		entry_game_msg_map( _msg, _proc )		case _msg##_id: {	_proc( __game_session__, (_msg*)__message__ ); break ;	}
#define		end_game_msg_map()						default: break; }

// bridge <-> game server class
class bridge_game_server : public bryllite::net::tcp_server
{
	using tcp_session = bryllite::net::tcp_session;
	using message_header = bryllite::net::message_header;

	// game session
	class game_session : public tcp_session
	{
	protected:
		time_t _heartbeat;

	public:
		game_session( boost::asio::io_service& ios ) : tcp_session( ios ), _heartbeat( 0 ) {
		};
	};

public:
	bridge_game_server( boost::asio::io_service& ios, unsigned short port ) : tcp_server( ios, port ) {
		log( "bridge_game_server started on port: %d", port );
	};

protected:

	// user_session allocator
	tcp_session* new_session( void ) {
		return new game_session( _ios );
	};

	// new connection
	virtual void onAccept( tcp_session* session ) {
		log( "bridge_game_server::onAccept()" );
	};

	// disconnected
	virtual void onDisconnect( tcp_session* session ) {
		log( "bridge_game_server::onDisconnected()" );
	};

	// write completed
	virtual void onWrite( tcp_session* session, size_t bytes_transferred ) {
		log( "bridge_game_server::onWrite() >> %d bytes sent", bytes_transferred );
	};

	// message received
	virtual void onMessage( tcp_session* session, message_header* msg ) {
		log( "bridge_game_server::onMessage() >> message=%d, message.size=%d", msg->msgid(), msg->size() );

		game_session* session_ = ( game_session* )session;
		bridge_message* msg_ = ( bridge_message* )msg;

		begin_game_msg_map( session_, msg_ )
			entry_game_msg_map( bridge_game_message_ping, onPing )
			entry_game_msg_map( bridge_game_message_pong, onPong )
			end_game_msg_map()

	};

	void onPing( game_session* session, bridge_game_message_ping* msg )
	{
		log( "bridge_game_server::onPing()" );
	};

	void onPong( game_session* session, bridge_game_message_pong* msg )
	{
		log( "bridge_game_server::onPong()" );
	};
};
