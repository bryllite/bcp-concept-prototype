#include "stdafx.hpp"

#include "node_server.hpp"


const int max_peer_cnt = 5;
PeerData g_peers[] = { 
	PeerData( 0, "127.0.0.1", 8000 ),
	PeerData( 1, "127.0.0.1", 8001 ),
	PeerData( 2, "127.0.0.1", 8002 ),
	PeerData( 3, "127.0.0.1", 8003 ),
	PeerData( 4, "127.0.0.1", 8004 )
};

static bool g_stop = false;

#ifdef _WIN32
BOOL WINAPI sig_handler( DWORD sig ) 
{
	switch( sig )
	{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
			dlog( "CRTL+C or CTRL_CLOSE received." );
			g_stop = true;
			break;
	}

	return TRUE;
};
#else //_WIN32
void sig_handler( int sig ) 
{
	switch( sig )
	{
		case SIGKILL:		// SIGKILL
		case SIGINT:		// CTRL-C
			dlog( "SIGKILL or SIGINT received." );
			g_stop = true;
			break;
	}
};
#endif //_WIN32

void register_signal( void ) 
{
	// console handler
#ifdef _WIN32
	SetConsoleCtrlHandler( sig_handler, TRUE );
#else //_WIN32
	signal( SIGINT, sig_handler );
	signal( SIGKILL, sig_handler );
#endif //_WIN32
};


// timer test
int main( int argc, char** argv ) 
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// ctrl+c
	register_signal();

	log( "Hello, Bryllite! %s", bryllite::version() );

	std::vector< std::unique_ptr< CNodeServer > > _node_servers;
	for( size_t i = 0 ; i < max_peer_cnt ; i++ )
	{
		PeerData& peer_data = g_peers[i];
		CNodeServer* pNodeServer = new CNodeServer();

		// append peers
		for( size_t idx = 0 ; idx < max_peer_cnt ; idx++ )
			pNodeServer->append_peer( g_peers[idx] );

		// start node server
		pNodeServer->start_server( peer_data );

		// connect to peers
		pNodeServer->connect_peers();

		// add node_server list
		std::unique_ptr< CNodeServer > node_server( pNodeServer );
		_node_servers.push_back( std::move( node_server ) );
	}

	while( !g_stop )
	{
		bryllite::sleep_for( 10 );

		for( auto& node : _node_servers )
			node->main();
	}

	for( auto& node : _node_servers )
		node->stop_server();

	log( "node-server-test terminated" );

	return 0 ;
}


