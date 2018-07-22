#include "game_server_lib.hpp"


// game server app
class game_server_app final 
{
private:
	int _argc ;
	char** _argv ;
	int _exitCode ;
	std::atomic<bool> _keepGoing ;
	static game_server_app* _app ;

	// app config
	app_config _app_config;

	// game server
	game_server* _game_server;

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
		// create some directories
		_app_config.create_directories();

		// set log file
		std::string logFileName = bryllite::format( "%s/%%y-%%m-%%d/game-server-%d.log", _app_config._log_dir.c_str(), _app_config.node_id() );
//		log_manager()->initialize(logFileName);
		bryllite::clog.logFile( logFileName );
		log( "Hello, Bryllite! %s", bryllite::version() );

		// start game server
		_game_server = new game_server(_app_config);
		_game_server->start_server();

		bryllite::timer ts ;
		while( keepGoing() ) 
		{
			bryllite::sleep_for(5);

			// run game server main proceudre
			_game_server->onMain();
		}

		// stop game server
		_game_server->stop_server();
		delete _game_server;

		return 0;
	};

	void on_signal( int sig ) {
		keepGoing(false);
	};

	// process command line options
	// returns: >= 0 : app go on
	// returns: < 0 : app stop
	int process_options( void )
	{
		namespace po = boost::program_options;

		// program options
		try
		{
			po::options_description desc( "Program Usage", 1024, 512 );
			desc.add_options()
				( "node-id,i", po::value<int>( &(_app_config._node_id) )->required(), "set node id" )
//				( "node-limit,l", po::value<size_t>( &(_app_config._max_node_count) ), "set max node limits" )
				( "config,c", po::value<std::string>( &_app_config._config_file )->default_value( "bryllite.config.json" ), "set config file path" )
				( "version,v", "show version information" )
				( "help,h", "show help message" );

			po::variables_map vm;
			po::store( po::parse_command_line( _argc, _argv, desc ), vm );
			po::notify( vm );

			if( vm.count( "help" ) )
			{
				std::cout << desc << std::endl;
				return -1;
			}

			if( vm.count( "version" ) )
			{
				std::cout << "Bryllite Game Server version: " << bryllite::version() << std::endl;
				std::cout << "Bryllite, Ltd. All Rights Reserved" << std::endl;
				return -1;
			}

			if ( !_app_config.loadConfig() )
			{
				std::cout << "app_config::loadConfig() failed" << std::endl;
				return -2;
			}
		}
		catch( std::exception& e )
		{
			std::cerr << "Error: " << e.what() << std::endl;
			return -100;
		}
		catch( ... )
		{
			std::cerr << "Unknown Error!" << std::endl;
			return -101;
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

	return 0 ;
}
