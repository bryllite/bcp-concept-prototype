#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

//#include "message.inl"

/*
// namespace bryllite::net
namespace bryllite { namespace net {

class tcp_client : bryllite::threadable
{
	using tcp = boost::asio::ip::tcp ;
	enum { read_buffer_size = 4096 } ;

public:
	tcp_client() : m_socket( m_ios ), m_ep( tcp::v4(), 0 ) {
	};

	tcp_client( const char* host, unsigned short port ) 
		: m_socket( m_ios ), m_ep( boost::asio::ip::address::from_string( host ), port ) {

		run_thread_for( nullptr ) ;
	};

	virtual ~tcp_client() {
		close() ;
	};

public:
	void send( bryllite::net::message& msg ) {
		if ( m_socket.is_open() ) {
			boost::system::error_code error ;
			boost::asio::write( m_socket, boost::asio::buffer( (char*)msg.buffer(), msg.size() ), error ) ;
		}
	};

	void connect( const char* host, unsigned short port ) {
		m_ep = tcp::endpoint( boost::asio::ip::address::from_string( host ), port ) ;

		run_thread_for( nullptr ) ;
	};

	void close( void ) {

		keepGoing(false) ;

		if ( m_socket.is_open() ) {
			m_socket.shutdown( tcp::socket::shutdown_both ) ;
			m_socket.close() ;
		}

		join();	// wait for thread terminated
	};

protected:
	virtual void onConnected( const boost::system::error_code& error )
	{
		dlog( "connected: %s", ( error == 0 ) ? "true" : "false" );
	};

	virtual void onDisconnected( const boost::system::error_code& error )
	{
		if( error == boost::asio::error::eof ) {
			wlog( "disconnected by host" );
		}
		else {
			wlog( "disconnected. error=(%d:%s)", error.value(), error.message() );
		}
	};

	virtual void onReceive( bryllite::net::message msg, const boost::system::error_code& error ) {
		log( "onReceive(): msg=%s, msg.len=%d, error=(%d:%s)", msg.payload(), msg.size(), error.value(), error.message() ) ;
	};

	int onThreadMain( bryllite::ThreadContext* pContext ) 
	{
		boost::system::error_code error ;

		m_socket.connect( m_ep, error ) ;
		onConnected( error ) ;

		if ( !error ) 
		{
			keepGoing(true) ;

			while( keepGoing() ) {
				
				// clear buffer
				memset( m_buffer, 0, sizeof( m_buffer ) ) ;

				// read message_header
				size_t bytes_read = boost::asio::read( m_socket, boost::asio::buffer( m_buffer, read_buffer_size ), 
					boost::asio::transfer_exactly( sizeof( message_header ) ), error ) ;
				if ( error ) {
					onDisconnected(error) ;
					break ;
				}

				// valid header?
				message_header* msg_header = reinterpret_cast<message_header*>(m_buffer) ;
				if ( msg_header->valid() ) 
				{
					// ready message_body
					boost::asio::read( m_socket, boost::asio::buffer( m_buffer + sizeof( message_header ), read_buffer_size - sizeof(message_header) ),
						boost::asio::transfer_exactly( msg_header->size() - sizeof( message_header ) ), error ) ;
					if ( error ) {
						onDisconnected( error ) ;
						break ;
					}

					// onReceive callback
					message* msg = reinterpret_cast<message*>(m_buffer) ;
					if ( msg->valid() ) {
						onReceive( *msg, error );	// copy 
					}
				} else {
					elog( "invalid message_header!" ) ;
					break ;
				}
			}
		} else {
			elog( "connect() failed! error=(%d:%s)", error.value(), error.message() ) ;
		}

		return 0 ;
	};

protected:
	boost::asio::io_service m_ios ;
	tcp::socket m_socket ;
	tcp::endpoint m_ep ;
	char m_buffer[ read_buffer_size + 1 ] ;
};

}};//namespace bryllite::net
*/

/*
class tcp_client 
{
	using tcp = boost::asio::ip::tcp ;
public:
	tcp_client( boost::asio::io_service& ios, const char* host, unsigned short port ) : m_ios(ios), m_socket(ios), m_ep( boost::asio::ip::address::from_string(host), port ) {

		m_socket.open( m_ep.protocol() ) ;

		// ios worker
		m_thread.reset( new std::thread([this](){
			m_ios.run() ;
		}));

		connect() ;
	};

	void connect( void ) {
		try {
			m_socket.async_connect( m_ep, [this]( const boost::system::error_code& ec ) {
				log( "on_connected" ) ;
				onConnected( ec != 0 );
			});
		}
		catch( boost::system::system_error& e ) {
			elog( "tcp_client::connect() failed. exception='%s(%d)'", e.what(), e.code() );
		}
	};

protected:
	virtual void onConnected( bool connected ) {
		log( "onConnected(%s) started", connected?"true":"false" ) ;
	};


protected:
	boost::asio::io_service& m_ios ;
	tcp::endpoint m_ep ;
	tcp::socket m_socket ;

	std::unique_ptr< std::thread> m_thread ;
};
*/

std::atomic<bool> g_stop = false ;

#ifdef _WIN32
BOOL WINAPI sig_handler( DWORD sig ) {
	switch( sig )
	{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
			wlog( "CRTL+C or CTRL_CLOSE received." );
			g_stop.store(true) ;
			break;
	}

	return TRUE;
};
#else //_WIN32
void sig_handler( int sig ) {
	switch( sig )
	{
		case SIGKILL:		// SIGKILL
		case SIGINT:		// CTRL-C
			wlog( "SIGKILL or SIGINT received." );
			g_stop.store(true);
			break;
	}
};
#endif //_WIN32


/*
class tcp_message_client : public bryllite::net::tcp_client 
{
public:
	tcp_message_client( const char* host, unsigned short port ) : bryllite::net::tcp_client( host, port ) {
	};

public:
	void onConnected( const boost::system::error_code& error ) {
		bool connected = ( error == 0 ) ;
		log( "onConnected: %s", connected?"true":"false" );
	} ;

	void onDisconnected( const boost::system::error_code& error ) {
		log( "onDisconnected: error=%d", error ) ;
	};

	void onReceive( bryllite::net::message msg, const boost::system::error_code& error ) {
		log( "onReceive: msg=%s", msg.payload() ) ;

		namespace pt = boost::property_tree ;

		std::stringstream ss( msg.payload() ) ;
		pt::ptree root ;
		pt::json_parser::read_json( ss, root ) ;

		std::string name = root.get<std::string>("name") ;
		int age = root.get<int>( "age" );
		log( "root.name=%s, root.age=%d", name.c_str(), age ) ;
	};
};
*/

const unsigned short tcp_port = 8000;
const unsigned short udp_port = 9000;

// tcp/udp echo client
int main( int argc, char** argv ) 
{
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

	using tcp = boost::asio::ip::tcp;

	log( "Hello, Bryllite! %s", bryllite::version() ) ;

	/*
	try {
		const char* request = "{\"name\":\"keith\", \"age\":42}" ;

		tcp_message_client client( "127.0.0.1", tcp_port );
		while( !g_stop.load() ) {
			namespace pt = boost::property_tree ;

			bryllite::sleep_for(10);

			pt::ptree root ;
			root.put( "name", "okseok.kim" ) ;
			root.put( "age", 42 ) ;
			root.put( "sex", "male" ) ;

			std::stringstream ss ;
			pt::json_parser::write_json( ss, root ) ;

			bryllite::net::message msg ;
			msg.set( ss.str() ) ;
			client.send( msg ) ;
		}
	} catch( std::exception e ) {
		elog( "Exception: %s", e.what() ) ;
	}
	*/

	return 0 ;
}
