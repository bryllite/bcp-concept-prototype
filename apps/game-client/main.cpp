#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "game_client.hpp"


// game client app
class game_client_app final
{
private:
	int _argc;
	char** _argv;
	int _exitCode;
	std::atomic<bool> _keepGoing;
	static game_client_app* _app;

	boost::asio::io_context _ioc;
	std::vector< std::shared_ptr<game_client> > _clients ;

	std::string _game_host, _node_host;
	unsigned short _game_port, _node_port;
	unsigned short _client_count;

public:
	game_client_app( int argc, char** argv ) : _argc( argc ), _argv( argv ), _exitCode( 0 ), _keepGoing( true ), _game_port(0), _node_port(0), _client_count(0) 
	{
		_app = this;

		// register signal
		register_signal();

		// process options
		if( 0 == process_options() )
		{
			// create N clients
			for( int i = 0; i<_client_count; i++ )
			{
				try
				{
					std::shared_ptr<game_client> cli( new game_client(_ioc) );
					cli->start_client( _game_host.c_str(), _game_port, _node_host.c_str(), _node_port );

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


		}

		for( auto& cli : _clients )
			cli->stop_client();

		return 0;
	};

	void on_signal( int sig ) {
		keepGoing( false );
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
				( "game-host,g", po::value<std::string>( &_game_host )->default_value( "127.0.0.1" ), "set game host ip address" )
				( "game-port,p", po::value<unsigned short>( &_game_port )->default_value( game_default_port ), "set game port number" )
				( "node-host,m", po::value<std::string>( &_node_host )->default_value( "127.0.0.1" ), "set node host ip address" )
				( "node-port,n", po::value<unsigned short>( &_node_port )->default_value( node_game_default_port ), "set node port number" )
				( "client-cnt,c", po::value<unsigned short>( &_client_count )->default_value( 1 ), "set client count" )
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
				std::cout << "Bryllite Game Client version: " << bryllite::version() << std::endl;
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

	// make memory leak
	int* p = new int ;
	*p = 5 ;

	return 0 ;
}

/*
pt::ptree root;
root.put( "name", "okseok.kim" );
root.put( "age", 42 );
root.put( "sex", false );
root.put( "height", 174.5 );

pt::ptree child1, child2;
child1.put( "name", "goeun.kim" );
child1.put( "age", 6 );
child1.put( "sex", true );
child1.put( "height", 120.6f );
child2.put( "name", "dajung.kim" );
child2.put( "age", 2 );
child2.put( "sex", true );
child2.put( "height", 90.4f );

pt::ptree childs ;
childs.push_back( std::make_pair("", child1) ) ;
childs.push_back( std::make_pair("", child2) ) ;
root.add_child( "child", childs ) ;

std::stringstream ss;
pt::json_parser::write_json( ss, root );

bryllite::net::message msg( ss.str() );
*/
