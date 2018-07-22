#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>
#include <iostream>
#include <atomic>

#include <queue>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include <boost/thread/mutex.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>


template< typename T >
class locked_queue
{
public:
	void push( T val ) {
		boost::mutex::scoped_lock lock(mutex) ;
		m_queue.push( val ) ;
	};

	T pop( void ) {
		boost::mutex::scoped_lock lock(mutex) ;
		T val ;
		std::swap( val, m_queue.front() ) ;
		m_queue.pop() ;
		return val ;
	};

	bool empty( void ) {
		boost::mutex::scoped_lock lock(mutex) ;
		return m_queue.empty() ;
	};

	size_t size( void ) {
		return m_queue.size() ;
	};
private:
	boost::mutex m_mutex ;
	std::queue<T> m_queue ;
};


class udp_server
{
	typedef boost::asio::ip::udp udp;
	typedef boost::asio::ip::udp::socket socket;
	typedef boost::asio::ip::udp::endpoint endpoint;
public:

	udp_server( boost::asio::io_context& io_context ) : m_socket( io_context, endpoint( udp::v4(), 13 ) ) {
		do_receive() ;
	};

protected:
	void do_receive( void ) {
		m_socket.async_receive_from( 
			boost::asio::buffer( m_recv_buffer ), m_remote_endpoint, 
			boost::bind( &udp_server::handle_receive, this, boost::asio::placeholders::error )) ;
	};

	void handle_receive( const boost::system::error_code& error ) {
		if ( !error || error == boost::asio::error::message_size ) {
		}
	};



protected:
	socket m_socket ;
	endpoint m_remote_endpoint ;
	boost::array< char, 1 > m_recv_buffer ;
};

/*
class App : public bryllite::application
{
public:
	App() {
	};

protected:
	int onThreadMain( bryllite::ThreadContext* pContext ) {
		bryllite::application::AppContext* pAppContext = reinterpret_cast<bryllite::application::AppContext*>(pContext) ;

		keepGoing(true) ;

		bryllite::timer ts;
		while( keepGoing() ) {
			log( "awake! laptime=%d", ts.laptime() ) ;

			bryllite::sleep_for( 100 ) ;
		};

		return 0 ;
	};

};
*/

// application class
class application : public bryllite::lockable, 
							public bryllite::threadable, 
							public bryllite::singleton< application >
{

	class AppContext : bryllite::ThreadContext 
	{
		friend class application ;
	public:
		AppContext( int _argc, char** _argv ) : argc(_argc), argv(_argv) {
		};

	protected:
		int argc ;
		char** argv ;
	};

	friend class bryllite::singleton< application > ;
protected:
	application() {
		onInitialize() ;
	};

	virtual ~application() {
		onCleanup() ;
	};

#ifdef _WIN32
	static BOOL WINAPI sig_handler( DWORD sig ) {
		switch( sig )
		{
			case CTRL_C_EVENT:
			case CTRL_CLOSE_EVENT:
				wlog( "CRTL+C or CTRL_CLOSE received." );
				application::instance()->stop( sig );
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
				wlog( "SIGKILL or SIGINT received." ) ;
				application::instance()->stop( sig );
				break;
		}
	};
#endif //_WIN32

	void onCleanup( void ) {
	};

	void onInitialize( void ){
		// check for memory leak
#ifdef _WIN32
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

		// console handler
#ifdef _WIN32
		SetConsoleCtrlHandler( sig_handler, TRUE );
#else //_WIN32
		signal( SIGINT, sig_handler );
		signal( SIGKILL, sig_handler );
#endif //_WIN32
	};

public:
	// initialize
	virtual bool initialize( void ) {

		return true;
	};

	// start
	virtual int start( int argc, char** argv, bool thread = false ) {

		if ( thread ) {
			run_thread_for( new AppContext( argc, argv ) ) ;
		} else {
			AppContext appContext( argc, argv ) ;
			if ( onThreadBegin( &appContext ) ) {
				int exitCode = onThreadMain( &appContext ) ;
				onThreadEnd( &appContext ) ;
				return exitCode ;
			}
		}

		return true ;
	};

	// stop
	virtual void stop( unsigned int sig ) {
		keepGoing( false ) ;
	};

protected:
	// before thread begin
	bool onThreadBegin( bryllite::ThreadContext* pContext ) {
		AppContext* pAppContext = static_cast< AppContext* >( pContext );

		std::stringstream args ;
		for( int i = 0 ; i < pAppContext->argc ; i++ ) {
			args << ((i==0)?"":" ") << pAppContext->argv[i] ;
		}

		log( "onThradBegin() STARTED. argc=%d, argv=[%s]", pAppContext->argc, args.str().c_str() ) ;

		return true ;
	};

	// thread main
	int onThreadMain( bryllite::ThreadContext* pContext ) {
		
		AppContext* pAppContext = static_cast< AppContext* >( pContext ) ;

		ilog( "onThreadMain() STARTED" ) ;

		keepGoing( true ) ;

		elog( "lock_exception. e='%x'", pContext ) ;
		wlog( "wanna this?" );
		dlog( "pContext=%x", pContext ) ;
		ulog( "user log" ) ;
/*
		std::string json( "packet message data" ) ;
		bryllite::net::message msg( json ) ;

		ilog( "msg.size=%d, msg.data=%s, msg.valid?=%d", msg.size(), msg.payload(), msg.valid() ) ;
*/
		bryllite::timer ts ;
		while( keepGoing() ) {

			try_smartlock( this, 100 ) 
				ilog( "awaked by timeout! laptime=%d(ms)", ts.laptime() ) ;
			catch_smartlock(e)
				wlog( "onThreadMain() lock_exception: e=%s", e.what() ) ;
			end_smartlock()

			bryllite::sleep_for( 10 ) ;
		};

		dlog( "onThreadMain() TERMINATED" ) ;

		return 0 ;
	};

protected:
	
};


class udp_client
{
public:
	udp_client( boost::asio::io_service& ios, const std::string host, const std::string port ) 
		: m_ios( ios ), m_socket( ios, boost::asio::ip::udp::endpoint( boost::asio::ip::udp::v4(), 0 ) ) {
		
		boost::asio::ip::udp::resolver resolver( m_ios ) ;
		boost::asio::ip::udp::resolver::query query( boost::asio::ip::udp::v4(), host, port ) ; 
		boost::asio::ip::udp::resolver::iterator iter = resolver.resolve( query ) ;
		m_endpoint = *iter ;
	};

	~udp_client( void ) {
		m_socket.close() ;
	};

	void send( const std::string& msg ) {
		m_socket.send_to( boost::asio::buffer( msg, msg.size() ), m_endpoint ) ;
	};

protected:
	boost::asio::io_service& m_ios ;
	boost::asio::ip::udp::socket m_socket ;
	boost::asio::ip::udp::endpoint m_endpoint ;
};

class tcp_endpoint {
public:
	tcp_endpoint( const char* _host, unsigned short _port ) {
		
	};


protected:
	boost::asio::ip::tcp::endpoint m_endpoint ;
};


// udp client application
class udp_client_app : public bryllite::threadable 
{
	class AppContext : public bryllite::ThreadContext 
	{
	public:
		AppContext( std::string _host, std::string _port ) : host(_host), port(_port) {
		};
		
		std::string host ;
		std::string port ;	
	};

public:
	udp_client_app() {
	};

protected:
	int onThreadMain( bryllite::ThreadContext* pContext ) {
		AppContext* pAppContext = static_cast< AppContext* >( pContext ) ;

		boost::asio::io_service ios ;
		udp_client client( ios, "localhost", "1337" ) ;

		int sendCount = 0 ;

		keepGoing( true ) ;
		while( keepGoing() ) {

			client.send( bryllite::format( "Hello, Bryllite! [%d]", sendCount++ ) ) ;

			bryllite::sleep_for( 1000 ) ;
		};

	};

};





// program entry
int main( int argc, char** argv ) 
{
/*
	application::instance()->initialize() ;

	// initialize log
	log_initialize( "test1-$DATE.log", bryllite::log_attr::all, bryllite::log_level::dev );

	// start application
	application::instance()->start( argc, argv, true ) ;

	// wait for application
	application::instance()->join();
*/

/*
	App::instance()->initialize() ;
	// initialize log
	log_initialize( "test1-$DATE.log", bryllite::log_attr::all, bryllite::log_level::dev );
	App::instance()->start( argc, argv, true ) ;
	App::instance()->join() ;
	*/

	return 0 ;
}
