#pragma once

#include "bridge_server.hpp"


// bridge server app
class bridge_server_app final
{
private:
	int _argc;
	char** _argv;
	int _exitCode;
	std::atomic<bool> _keepGoing;
	static bridge_server_app* _app;

	bridge_server _bridge_server;

public:
	bridge_server_app( int argc, char** argv )
		: _argc( argc ), _argv( argv ), _exitCode( 0 ), _keepGoing( true )
	{
		_app = this;

		register_signal();

		// program options
		if( 0 == process_options() )
		{
			// call main
			_exitCode = _main( _argc, _argv );
		}
	};

	int exitCode( void ) { return _exitCode; };
	bool keepGoing( void ) { return _keepGoing.load(); };
	void keepGoing( bool go ) { _keepGoing.store( go ); };

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
	int _main( int argc, char** argv );

	void on_signal( int sig ) {
		keepGoing( false );
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
