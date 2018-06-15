#include <cstdio>
#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include "node_manager.hpp"
#include "node.hpp"


node_manager _node_manager;

// timer test
int main( int argc, char** argv ) 
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
/*
	if ( argc != 2 ) {
		std::cout << "usage: node_test node_count" << std::endl;
		std::cout << "ex: node_test 5" << std::endl;
		return 0;
	}
*/

	const int max_node_cnt = 100;
	const int default_node_cnt = 3;

	int node_cnt = argc>=2?atoi( argv[1] ): default_node_cnt;
	if ( node_cnt <= 0 || node_cnt > max_node_cnt ) {
		node_cnt = default_node_cnt;
	}

	log( "Hello, Bryllite! %s", bryllite::version() ) ;

	node* _nodes[max_node_cnt+1];
	memset( _nodes, 0, sizeof(_nodes) );

	for( int i=0; i<node_cnt; i++ )
	{
		_nodes[i] = new node(_node_manager);
		_nodes[i]->start(i);
	}

	_node_manager.run();

	for( int i=0; i<max_node_cnt; i++ )
	{
		if ( _nodes[i] )
			delete _nodes[i];
	}

	return 0 ;
}


