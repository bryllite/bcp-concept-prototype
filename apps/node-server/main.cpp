#include <iostream>
#include <string>
#include <cstdint>

#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>

#include "node_server.hpp"
#include "app_config.hpp"






// node server app
class node_server_app final 
{
	using udp = boost::asio::ip::udp;
	using tcp = boost::asio::ip::tcp;

private:
	int _argc ;
	char** _argv ;
	int _exitCode ;
	std::atomic<bool> _keepGoing ;
	static node_server_app* _app ;

	// node server class
	node_server _node_server;

	// app config
	app_config _app_config;

	// node server
//	node_server _node_server;
//	unsigned short _node_port;
//	size_t _ping_id;

	// bridge client
//	boost::asio::io_service _ios_bridge;
//	bridge_client _bridge_client;
//	std::string _bridge_host;
//	unsigned short _bridge_port;

	// config filename
	std::string _config_file;

public:
	node_server_app( int argc, char** argv ) 
		: _node_server( _app_config ), _argc(argc), _argv(argv), _exitCode(0), _keepGoing(true)//, _ping_id(0), _bridge_client( _ios_bridge )
	{
		_app = this ;

		// register signal
		register_signal();

		// program options
		if ( 0 == process_options() )
		{
			// call main
			_exitCode = _main( _argc, _argv );
		}
	};

	int exitCode( void ) { return _exitCode; };
	bool keepGoing(void) { return _keepGoing.load(); };
	void keepGoing(bool go) { _keepGoing.store(go); };

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


	// main function
	int _main( int argc, char** argv ) 
	{
		log( "Hello, Bryllite! %s", bryllite::version() );

		// start node server
		_node_server.start_server();
		
		while( keepGoing() ) 
		{
			bryllite::sleep_for( 0 );

			// call node_server main()
			_node_server.main();
		}

		// stop node server
		_node_server.stop_server();

		return 0;
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
				( "node-port,p", po::value<unsigned short>( &(_app_config._peer_port) ), "set node p2p port number" )
				( "config,c", po::value<std::string>( &_config_file )->default_value( "config.json" ), "set config file path" )
				( "key-file,k", po::value<std::string>( &_app_config._key_file )->default_value( "keyfile.key" ), "set config file path" )
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
				std::cout << "Bryllite Node Server version: " << bryllite::version() << std::endl;
				std::cout << "Bryllite, Ltd. All Rights Reserved" << std::endl;
				return 1;
			}

			// read config file
			return read_config();
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

	// read config file
	int read_config( void )
	{
		namespace pt = boost::property_tree;

		std::cout << "current path: " << boost::filesystem::current_path() << "config_file: " << _config_file << std::endl;

		// file exists?
		if ( !boost::filesystem::exists( _config_file ) )
		{
			std::cerr << "config file not found" << std::endl;
			return -1;
		}

		pt::ptree root;
		pt::read_json( _config_file, root );

		// node id
		_app_config._node_id = root.get<int>("id");
		_app_config._peer_port = _app_config._peer_port > 0 ? _app_config._peer_port : root.get< unsigned short >("peer-port");
		_app_config._bridge_port = root.get< unsigned short >("bridge-port");
		_app_config._game_port = root.get< unsigned short >("game-port");

		for( auto& peer : root.get_child( "peers" ) )
		{
			int peerId = peer.second.get<int>("id");
			if ( peerId == _app_config._node_id )
				continue ;

			std::string host = peer.second.get< std::string >("host");
			unsigned short port = peer.second.get< unsigned short >("port");

			_app_config._peers.push_back( udp::endpoint( boost::asio::ip::address::from_string( host.c_str() ), port ) );
		}

		return 0;
	};

	void on_signal( int sig ) {
		keepGoing(false);
	};

protected:
#ifdef _WIN32
	static BOOL WINAPI sig_handler( DWORD sig ) {
		switch( sig )
		{
			case CTRL_C_EVENT:
			case CTRL_CLOSE_EVENT:
				dlog( "CRTL+C or CTRL_CLOSE received." );
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
				dlog( "SIGKILL or SIGINT received." );
				_app->on_signal( sig );
				break;
		}
	};
#endif //_WIN32
};
node_server_app* node_server_app::_app=nullptr ;


// main entry point
int main( int argc, char** argv )
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// start node-server application
	// will be block here( see game_server_app::_main() )
	node_server_app app( argc, argv ) ;

	log( "node-server terminated: exitCode=%d", app.exitCode() ) ;

	// make memory leak!
	int* p = new int ;
	*p = 0 ;

	return *p ;
}
