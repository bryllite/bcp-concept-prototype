#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "game_server.hpp"


// game server app
class game_server_app final 
{
private:
	int _argc ;
	char** _argv ;
	int _exitCode ;
	std::atomic<bool> _keepGoing ;
	static game_server_app* _app ;

	std::string _bridge_host;
	unsigned short _bridge_port, _game_port;

	// game server
	game_server _game_server;

public:
	game_server_app( int argc, char** argv ) 
		: _argc( argc ), _argv( argv ), _exitCode( 0 ), _keepGoing(true) 
	{
		_app = this ;

		register_signal();

		// program options
		if( 0 == process_options() )
		{
			// call main
			_exitCode = _main( _argc, _argv );
		}
	};

	int exitCode( void ) { return _exitCode; };
	bool keepGoing(void) { return _keepGoing.load(); };
	void keepGoing(bool go) { _keepGoing.store(go); };

	void register_signal( void ) {
		// console handler
#ifdef _WIN32
		SetConsoleCtrlHandler( sig_handler, TRUE );
#else //_WIN32
		signal( SIGINT, sig_handler );
		signal( SIGKILL, sig_handler );
#endif //_WIN32
	};

	// main function
	int _main( int argc, char** argv ) 
	{
		// start game server
		_game_server.start_server( _bridge_host.c_str(), _bridge_port, _game_port );

		bryllite::timer ts ;
		while( keepGoing() ) {
			bryllite::sleep_for( 0 );

		}

		// stop game server
		_game_server.stop_server();

		return 0;
	};

	void on_signal( int sig ) {
		keepGoing(false);
	};

	// process command line options
	// return 0 : app go on
	// return !0 : app stop
	int process_options( void )
	{
		namespace po = boost::program_options;

		// program options
		try
		{
			po::options_description desc( "Program Usage", 1024, 512 );
			desc.add_options()
				( "game-port,g", po::value<unsigned short>( &_game_port )->default_value( game_default_port ), "set game port number" )
				( "bridge-host,b", po::value<std::string>( &_bridge_host )->default_value( "127.0.0.1" ), "set bridge host ip" )
				( "bridge-port,p", po::value<unsigned short>( &_bridge_port )->default_value( bridge_game_default_port ), "set bridge port number" )
				( "version,v", "show version information" )
				( "help,h", "show help message" );

			po::variables_map vm;
			po::store( po::parse_command_line( _argc, _argv, desc ), vm );
			po::notify( vm );

			if( vm.count( "help" ) )
			{
				std::cout << desc << std::endl;
				return 1;
			}

			if( vm.count( "version" ) )
			{
				std::cout << "Bryllite Game Server version: " << bryllite::version() << std::endl;
				std::cout << "Bryllite, Ltd. All Rights Reserved" << std::endl;
				return 1;
			}

			return 0;
		}
		catch( std::exception& e )
		{
			std::cerr << "Error: " << e.what() << std::endl;
			return 2;
		}
		catch( ... )
		{
			std::cerr << "Unknown Error!" << std::endl;
			return 3;
		}

		return 0;
	};

protected:
#ifdef _WIN32
	static BOOL WINAPI sig_handler( DWORD sig ) {
		switch( sig )
		{
			case CTRL_C_EVENT:
			case CTRL_CLOSE_EVENT:
				wlog( "CRTL+C or CTRL_CLOSE received." );
				_app->on_signal( sig ) ;
				break;
		}

		return TRUE;
	};
#else //_WIN32
	static void sig_handler( int sig ) {
		switch( sig )
		{
			case SIGKILL:		// SIGKILL
			case SIGINT:		// CTRL-C
				wlog( "SIGKILL or SIGINT received." );
				_app->on_signal( sig );
				break;
		}
	};
#endif //_WIN32
};
game_server_app* game_server_app::_app=nullptr ;



// main entry point
int main( int argc, char** argv )
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// game server app 
	// will be block here.
	game_server_app app( argc, argv );


	log( "game-server terminated: exitCode=%d", app.exitCode() ) ;

	// make memory leak!
	int* p = new int ;
	*p = 0 ;

	return *p ;
}
