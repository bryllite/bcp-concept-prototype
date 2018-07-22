#include "stdafx.hpp"
#include "node_server.hpp"

node_manager g_node_manager;

#ifdef _WIN32
BOOL WINAPI sig_handler( DWORD sig ) 
{
	switch( sig )
	{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
			dlog( "CRTL+C or CTRL_CLOSE received." );
			g_node_manager.stop();
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
			g_node_manager.stop();
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


int main( int argc, char** argv )
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	log( "hello, bryllite! %s", bryllite::version() );

	// register signal
	register_signal();


	// start
	g_node_manager.start( 5 );

	// main
	g_node_manager.main();

	log( "node-server-mono terminated" );

	return 0;
}
