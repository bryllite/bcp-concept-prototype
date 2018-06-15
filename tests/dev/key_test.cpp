#include <cstdio>
#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

// key collision test
int main( int argc, char** argv ) 
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	log( "Hello, Bryllite! %s", bryllite::version() ) ;

	std::map< std::string, uint264 > keyMap;

	unsigned int idx = 0, collision = 0;
	bryllite::timer ts;
	while( true )
	{
		// create key pair
		bryllite::key_pair _key_pair;
		_key_pair.generate_new();

		// key address
		std::string addr = _key_pair.address();

		// check for collision
		if ( keyMap.count( addr ) > 0 ) 
		{
			std::cout << "[ERROR]!!: key collision! address=" << addr << std::endl;
			collision++ ;
		} else {
			keyMap.insert( std::make_pair( addr, _key_pair.get_public_key() ) );
		}

		// print collision status
		if ( ++idx % 100 == 0 ) {
			std::cout << "[" << idx << "] elapsed time=" << ts.laptime() << "(ms) keyCnt=" << keyMap.size() << ", collision=" << collision << std::endl;
		}
	};

	std::cout << "[" << idx << "] elapsed time=" << ts.laptime() << "(ms) keyCnt=" << keyMap.size() << ", collision=" << collision << std::endl;
	return 0;
}


