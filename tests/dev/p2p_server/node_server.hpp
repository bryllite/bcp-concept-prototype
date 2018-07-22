#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

#include "p2p_server.hpp"
#include "peer_message.inl"



class node_server : public p2p_server
{
protected:
	time_t _last_sec;
	size_t _ping_id;

public:
	node_server();
	boost::asio::io_service& ios( void );



	int main( void );

protected:
	// new connection
	void onAccept( p2p_session* session );

	// disconnected
	void onDisconnected( p2p_session* session );

	// message received
	void onMessage( p2p_session* session, message* msg );

	int onPeerMessagePing( p2p_session* session, message_ping* msg );
	int onPeerMessagePong( p2p_session* session, message_pong* msg );
	int onPeerMessagePeerNotify( p2p_session* session, node_message_peer_notify* msg );

};

