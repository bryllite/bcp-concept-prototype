#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

#include "p2p_client.hpp"


// node client
class node_client : public p2p_client
{
protected:

public:
	node_client(boost::asio::io_service& ios);

protected:
	virtual void onConnected( bool connected );
	virtual void onDisconnected( void );
	virtual void onReceived( message* msg );
	virtual void onWrited( size_t bytes_transferred );
};
