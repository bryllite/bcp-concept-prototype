#include <cstdio>
#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include "p2p_server.hpp"
#include "node_server.hpp"
#include "node_client.hpp"


// generate random string ( 100 - 199 )
std::string generate_random_string(void)
{
	std::stringstream ss;

	size_t length = 100 + (rand() % 100);
	for (size_t i = 0; i < length; i++)
	{
		ss << bryllite::byte_to_hex((byte)(rand() % 256));
	}

	return ss.str();
}

uint256 generate_random_256( void )
{
	uint256 s;

	byte* p = (byte*)&s;
	for( size_t i = 0 ; i < sizeof(uint256); i++ )
		p[i] = rand() % 256;

	return s;
}


void thread_worker( int idx, const char *host, unsigned short port )
{
	boost::asio::io_service ios;

	node_client* client = new node_client(ios);
	client->connect( host, port );

	std::thread t(boost::bind(&boost::asio::io_service::run, &ios ) );

	bryllite::sleep_for(1000);

	size_t ping_id = 0 ;
	bryllite::timer ts;
	while( client->connected() && ts.laptime() <= 600000 )
	{
		bryllite::sleep_for(10);

//		if ( rand() % 2 == 0 )
		{
			message_ping msg( ping_id++ );
			client->write( &msg );
		} 
	}

	ios.stop();

	t.join();
	delete client;

	log( "thread_worker[%d] terminated", idx );
};

// timer test
int main( int argc, char** argv ) 
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	log( "Hello, Bryllite! %s", bryllite::version() );

	const int peer_cnt = 3;
	const int max_peer_cnt = 5;
	peer_info peers[max_peer_cnt] = { 
		peer_info("node1", "127.0.0.1", 8000), 
		peer_info("node2", "127.0.0.1", 8001), 
		peer_info("node3", "127.0.0.1", 8002), 
		peer_info("node4", "127.0.0.1", 8003), 
		peer_info("node5", "127.0.0.1", 8004)
	};

	node_server nodes[peer_cnt];

	for( int i=0; i< peer_cnt; i++ )
	{
		nodes[i].start_server( peers[i].peer_id(), peers[i].host(), peers[i].port() );
		
		for( int j=0; j<peer_cnt; j++ )
		{
			if ( i == j ) continue ;

			nodes[i].append_peer( peers[j].peer_id(), peers[j].host(), peers[j].port() );
		}
	}

	for( int i=0; i<peer_cnt; i++)
		nodes[i].connect_to_peers();

	while( true )
	{
		bryllite::sleep_for(10);

		for( int i=0; i<peer_cnt; i++ )
			nodes[i].main();
	}

	for( int i=0; i<peer_cnt; i++ )
		nodes[i].stop_server();

	return 0;

	byte_stream bstream(1024);

	while( true )
	{
		size_t type = ( rand() % 5 );

		if ( type == 0 )
		{
			uint256 r256 = generate_random_256();
			bstream.write( (byte*)&r256, sizeof(r256) );
			dlog( "r256=%s", r256.GetHex().c_str() );

			std::string s256 = r256.GetHex();
			bstream << s256;
			dlog( "s256=%s", s256.c_str() );

			uint256 w256 ;
			bstream.read( (byte*)&w256, sizeof(w256) );
			dlog( "w256=%s", w256.GetHex().c_str() );

			char s[ 1024 ];
			memset( s, 0, sizeof(s) );
			bstream.read( (byte*)s, sizeof(s) );
			dlog( "s=%s", s );
		}
		else if ( type == 1 )
		{
			size_t cnt = rand() % 1024;

		}
	}

	return 0 ;
}


