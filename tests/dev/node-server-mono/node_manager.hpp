#pragma once

class node_server;
class node_manager : public bryllite::lockable
{
protected:
	std::map< int, std::unique_ptr<node_server> > _node_servers;
	bool _stop;

public:
	node_manager();

	// start node server
	bool start( size_t node_count );
	void stop( void );

	int main( void );

	size_t all_peer_count( void );

	node_server* get_node_server( int peer_id );

	size_t sendall( node_server* sender, message* msg );
};
