#include "node_manager.hpp"


node_manager::node_manager() : _stop(false)
{
}

bool node_manager::append_node( node* n )
{
	if ( !n ) return false;

	{
		smartlock(this);

		if ( exists(n) ) return false;
		_nodes.push_back( n );
	}

	return true;
}

bool node_manager::exists( node* n )
{
	_assert( n != nullptr, "node is null" );

	if ( !n ) return false;

	{
		smartlock(this);
		for( auto& nd : _nodes )
			if ( nd == n ) return true ;
	}

	return false;
}

int node_manager::run( void )
{
	while( !stop() ) 
	{
		bryllite::sleep_for(10);

		for(auto& n : _nodes ) 
		{
			// process main
			n->onMain();
		}
	}

	return 0;
}

void node_manager::sendto( node* receiver, message* msg, node* sender )
{
	if ( receiver && msg ) {
		receiver->onMessage( sender, msg );
	}
}

void node_manager::sendall( message* msg, node* sender )
{
	if ( !sender || !msg ) return ;

	for( auto& n : _nodes ) {
		n->onMessage( sender, msg );
	}
}

size_t node_manager::get_peer_count(void) 
{
	return _nodes.size();
}

bool node_manager::stop(void)
{
	return _stop; 
}

void node_manager::stop(bool s)
{
	_stop = s; 
}
