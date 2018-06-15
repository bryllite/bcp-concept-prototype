#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

#include "node.hpp"


// node manager class 
class node_manager : public bryllite::lockable
{
protected:
	bool _stop;
	std::vector< node* > _nodes;

public:
	// node manager ctor
	node_manager();

	// append node
	bool append_node( node* n );
	bool exists( node* n );

	// sendto/sendall
	void sendto( node* receiver, message* msg, node* sender );
	void sendall( message* msg, node* sender );

	// call node's main() every tick
	int run( void );

	// node count
	size_t get_peer_count(void);

	bool stop(void);
	void stop(bool s);
};
