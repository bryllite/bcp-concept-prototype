#include "peer.hpp"

#include "peer_message.inl"


CPeer::CPeer( boost::asio::io_service& ios, peer_info peer, peer_info me ) 
	: p2p_client(ios), _peer(peer), _me(me), _session(nullptr)
{
};

std::string CPeer::peer_id( void )
{
	return _peer.peer_id();
};

p2p_session* CPeer::session( void ) 
{
	return _session;
};

void CPeer::session( p2p_session* session )
{
	_session = session;
};

bool CPeer::connect_peer( void )
{
	if ( connected() ) return true;

	return p2p_client::connect( _peer.host().c_str(), _peer.port() );
};

void CPeer::onConnected( bool connected )
{
	log( "peer[%s]: connected %s", peer_id().c_str(), connected?"true":"false" );

	// notify me
	node_message_peer_notify noti( _me.peer_id().c_str(), _me.host().c_str(), _me.port() );
	write( &noti );
};

void CPeer::onDisconnected( void )
{
	log( "peer[%s]: disconnected", peer_id().c_str() );
};

void CPeer::onReceived( message* msg )
{
//	log( "peer[%s]: onReceived message. msg.id=%d, msg.size=%d", peer_id().c_str(), msg->msgid(), msg->size() );
};

void CPeer::onWrited( size_t bytes_transferred )
{
	log( "peer[%s]: write completed. bytes_transferred=%d", peer_id().c_str(), bytes_transferred );
};


bool CPeer::compare( peer_info peer )
{
	return _peer == peer;
};
