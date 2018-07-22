#include "game_client_lib.hpp"



// game client app
class game_client_app final
{
private:
	int _argc;
	char** _argv;
	int _exitCode;
	std::atomic<bool> _keepGoing;
	static game_client_app* _app;

	boost::asio::io_service _io_service;

	app_config _app_config;

	std::vector< std::shared_ptr<game_client> > _clients ;

	std::unique_ptr< std::thread > _io_worker_thread;


public:
	game_client_app( int argc, char** argv ) : _argc( argc ), _argv( argv ), _exitCode( 0 ), _keepGoing( true )
	{
		_app = this;

		// register signal
		register_signal();

		// process options
		if( process_options() >= 0 )
		{
			// create some directories
			_app_config.create_directories();

			// set log file
			std::string logFileName = bryllite::format( "%s/%%y-%%m-%%d/game-client-%d.log", _app_config._log_dir.c_str(), _app_config.node_id() );
//			log_manager()->initialize(logFileName);
			bryllite::clog.logFile( logFileName );
			log( "Hello, Bryllite! %s", bryllite::version() );

			// create N clients
			for( size_t i = 0; i<_app_config._client_cnt; i++ )
			{
				try
				{
					std::shared_ptr<game_client> cli( new game_client(_io_service, _app_config) );
					cli->start(i);

					_clients.push_back( cli );
				}
				catch( std::exception& e )
				{
					elog( "game_client_app::game_client_app() >> exception! e='%s'", e.what() );
				}
				catch ( ... )
				{
					elog( "game_client_app::game_client_app() >> unknown exception!" );
				}
			}

			log( "game-client emulator: %d client started!", _clients.size() );

			// io worker thread
			_io_worker_thread.reset( new std::thread( boost::bind( &boost::asio::io_service::run, &_io_service ) ) );

			// call main
			_exitCode = _main( _argc, _argv );
		}
	};

	int exitCode( void ) { return _exitCode; };
	bool keepGoing( void ) { return _keepGoing.load(); };
	void keepGoing( bool go ) { _keepGoing.store( go ); };

	// main function
	int _main( int argc, char** argv ) 
	{
		bryllite::timer ts;
		while( keepGoing() ) 
		{
			bryllite::sleep_for( 1000 );

			// call client's main procedure
			for( auto& cli : _clients )
				cli->onMain();
		}

		// stop io service
		_io_service.stop();

		// stop clients
		for( auto& cli : _clients )
			cli->stop();

		_io_worker_thread->join();

		return 0;
	};

	void on_signal( int sig ) {
		keepGoing( false );
	};

	// process command line options
	// returns >= 0 : app go on
	// returns < 0 : app stop
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
				( "client-count,n", po::value<size_t>( &_app_config._client_cnt )->default_value(1), "set client count" )
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
				std::cout << "Bryllite Game Client version: " << bryllite::version() << std::endl;
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
				_app->on_signal( sig );
				break;
		}

		return TRUE;
	};
	// register console ctrl handler
	void register_signal( void ) { 
		SetConsoleCtrlHandler( sig_handler, TRUE ); 
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

	// register signal
	void register_signal( void ) {
		signal( SIGINT, sig_handler );
		signal( SIGKILL, sig_handler );
	};

#endif //_WIN32
};
game_client_app* game_client_app::_app = nullptr;


int main( int argc, char** argv )
{
	namespace po = boost::program_options ;

	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	game_client_app app( argc, argv );

	log( "game-client() terminated. exitCode=%d", app.exitCode() ) ;

	return 0 ;
}

