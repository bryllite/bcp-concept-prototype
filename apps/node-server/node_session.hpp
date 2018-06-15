#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

// node session for peer
class node_session : public bryllite::net::peer_session
{
	friend class node_server;
protected:
	size_t _blockHeight;

public:
	node_session( udp::endpoint ep );
};
