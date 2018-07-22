#include "node_client.hpp"


node_client::node_client(boost::asio::io_service& ios) : p2p_client(ios)
{
};

void node_client::onConnected( bool connected )
{
	log( "node_client::onConnected() %s", connected?"true":"false" );
};

void node_client::onDisconnected( void )
{
	log( "node_client::onDisconnected()" );
};

void node_client::onReceived( message* msg )
{
	log( "node_client::onReceived(): msg.id=%d, msg.size=%d", msg->msgid(), msg->size() );
};

void node_client::onWrited( size_t bytes_transferred )
{
};
