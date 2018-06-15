#pragma once

#include "../bridge_message.inl"

// bridge-node message handler
#define		begin_node_msg_map( _session, _msg )	node_session* __node_session__=_session; bridge_message* __message__=_msg; switch( _msg->msgid() ) {
#define		entry_node_msg_map( _msg, _proc )		case _msg##_id: {	_proc( __node_session__, (_msg*)__message__ ); break ;	}
#define		end_node_msg_map()						default: break; }

class bridge_server_app;

// bridge <-> node server class
class bridge_node_server : public bryllite::net::tcp_server
{
	using tcp_session = bryllite::net::tcp_session;
	using message_header = bryllite::net::message_header;

protected:
	bridge_server_app& _server_app ;

	// node session
	class node_session : public tcp_session
	{
	protected:
		time_t _heartbeat;

	public:
		node_session( boost::asio::io_service& ios ) : tcp_session( ios ), _heartbeat( 0 )
		{
		};
	};

public:
	bridge_node_server( boost::asio::io_service& ios, unsigned short port, bridge_server_app& app ) : tcp_server( ios, port ), _server_app(app)
	{
		log( "bridge_node_server started on port: %d", port );
	};

protected:
	tcp_session* new_session( void ) {
		return new node_session( _ios );
	};

	// new connection
	virtual void onAccept( tcp_session* session )
	{
		node_session* session_ = ( node_session* )session;
		log( "bridge_node_server::onAccept()" );
	};

	// disconnected
	virtual void onDisconnect( tcp_session* session )
	{
		node_session* session_ = ( node_session* )session;
		log( "bridge_node_server::onDisconnected()" );
	};

	// message received
	void onMessage( tcp_session* session, message_header* msg );

	// write completed
	virtual void onWrite( tcp_session* session, size_t bytes_transferred )
	{
		node_session* session_ = ( node_session* )session;
		log( "bridge_node_server::onWrite() >> %d bytes sent", bytes_transferred );
	};

	// message handler
	void onPing( node_session* session, bridge_node_message_ping* msg );
	void onPong( node_session* session, bridge_node_message_pong* msg );
	void onTest( node_session* session, node_bridge_message_test* msg );
};