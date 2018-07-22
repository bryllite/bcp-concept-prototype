#include "stdafx.hpp"
#include "node_manager.hpp"


node_manager::node_manager() : _stop(false)
{
}


bool node_manager::start( size_t node_count )
{
	smartlock(this);
	for( size_t peer_id = 0 ; peer_id < node_count ; peer_id++ )
	{
		std::unique_ptr<node_server> peer( new node_server(*this, peer_id) );
		peer->start_server();

		_node_servers.insert( std::make_pair( peer_id, std::move(peer) ) );
	}

	return true;
}

void node_manager::stop( void )
{
	_stop = true;

	smartlock(this);
	for( auto& pair : _node_servers )
	{
		int peer_id = pair.first;
		node_server* peer = pair.second.get();

		peer->stop_server();
	}
}

int node_manager::main( void )
{
	while( !_stop )
	{
		bryllite::sleep_for(10);

		smartlock(this);
		for( auto& pair : _node_servers )
		{
			int peer_id = pair.first;
			node_server* peer = pair.second.get();

			peer->main();
		}
	}

	return 0;
}

size_t node_manager::all_peer_count( void )
{
	return _node_servers.size();
}

node_server* node_manager::get_node_server( int peer_id )
{
	if ( _node_servers.count( peer_id ) == 0 ) return nullptr; 
	return _node_servers[peer_id].get();
}

size_t node_manager::sendall( node_server* sender, message* msg )
{
	size_t send_cnt = 0;

	for( auto& pair : _node_servers )
	{
		int peer_id = pair.first;
		node_server* peer = pair.second.get();

		peer->onPeerMessage( sender, msg );
		send_cnt++;
	}

	return send_cnt;
}
