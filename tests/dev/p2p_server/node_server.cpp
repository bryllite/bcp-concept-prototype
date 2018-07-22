#include "node_server.hpp"

node_server::node_server() : _last_sec(0), _ping_id(0)
{
};

boost::asio::io_service& node_server::ios( void ) 
{
	return _ios;
};

// new connection
void node_server::onAccept( p2p_session* session )
{
	log( "node_server::onAccept" );
};

// disconnected
void node_server::onDisconnected( p2p_session* session )
{
	log( "node_server::onDisconnected" );
};

// peer message map for node message
#define		begin_peer_msg_map( _session, _msg )	p2p_session* __session__=_session; message* __message__=_msg; switch( _msg->msgid() ) {
#define		entry_peer_msg_map( _msg, _proc )		case _msg##_id: {	_proc( __session__, (_msg*)__message__ ); return ;	}
#define		end_peer_msg_map()						default: break; }


// message received
void node_server::onMessage( p2p_session* session, message* msg )
{
	log( "node_server::onMessage: msg.id=%d, msg.size=%d", msg->msgid(), msg->size() );

	begin_peer_msg_map( session, msg )
		entry_peer_msg_map( message_ping, onPeerMessagePing )
		entry_peer_msg_map( message_pong, onPeerMessagePong )
		entry_peer_msg_map( node_message_peer_notify, onPeerMessagePeerNotify )
	end_peer_msg_map()

};

int node_server::onPeerMessagePing( p2p_session* session, message_ping* msg )
{
	dlog( "node_server[%s]: message_ping. id=%d", _me.peer_id().c_str(), msg->_id );

	message_pong ack( msg->_id );
	session->write( &ack );

	return 0;
};

int node_server::onPeerMessagePong( p2p_session* session, message_pong* msg )
{
//	dlog( "node_server[%s]: message_pong. id=%d", _me.peer_id().c_str(), msg->_id );

	return 0;
};


int node_server::onPeerMessagePeerNotify( p2p_session* session, node_message_peer_notify* msg )
{
	log( "p2p_server::onPeerMessagePeerNotify()" ) ;

	CPeer* peer = find_peer( msg->_node_id );
	if ( !peer )
	{
		elog( "onPeerMessagePeerNotify(): find_peer() failed. node_id=%s", msg->_node_id );
		close_session( session );
		return 1;
	}

	if ( !peer->compare( peer_info( msg->_node_id, msg->_host, msg->_port ) ) )
	{
		elog( "onPeerMessagePeerNotify(): invalid peer information" );
		close_session( session );
		return 2;
	}


	// set peer session
	peer->session( session );

	// connect to peer
	peer->connect_peer();


	return 0;
};


int node_server::main( void )
{
	time_t sec = _ntp.timestamp();

	if ( sec != _last_sec )
	{
		_last_sec = sec;

		message_ping ping( _ping_id++ );
		sendall( &ping );
	}

	return 0;
};
