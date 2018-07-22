#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>
#include <iostream>
#include <atomic>

#include <queue>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include <boost/asio.hpp>


// tcp client for sync
class tcp_client 
{
public:
	tcp_client( const std::string raw_ip_address, unsigned short port ) 
		: m_endpoint( boost::asio::ip::address::from_string( raw_ip_address ), port ), m_socket( m_ios ) {
		m_socket.open( m_endpoint.protocol() ) ;
	};

	// connect
	bool connect( void ) {
		try {
			m_socket.connect( m_endpoint ) ;
			return true ;
		} catch ( boost::system::system_error& e ) {
			wlog( "connect failed: e=%s(%d)", e.what(), e.code() ) ;
		}

		return false ;
	};

	bool close( void ) {
		try {
			m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_both ) ;
			m_socket.close() ;
			return true ;
		}catch( boost::system::system_error& e ) {
			wlog( "close failed: e=%s(%d)", e.what(), e.code() ) ;
		}

		return false ;
	};

	bool sendMessage( const std::string& msg ) {
		boost::asio::write( m_socket, boost::asio::buffer( msg ) ) ;
	};
protected:
	boost::asio::io_service m_ios ;
	boost::asio::ip::tcp::endpoint m_endpoint ;
	boost::asio::ip::tcp::socket m_socket ;
};

/*
// namespace bryllite::net
namespace bryllite { namespace net {

class tcp_client //: public lockable, public threadable
{
public:
	tcp_client() : m_socket( m_ios ) {

		m_work.reset( new boost::asio::io_service::work( m_ios ) ) ;

		m_thread.reset( new std::thread( [this]() {
			m_ios.run() ;
		}));
	};

	bool connect( std::string host, unsigned short port ) {
		boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string( host ), port ) ;

		try {
			m_socket.open( ep.protocol() ) ;
			m_socket.async_connect( ep, [this](const boost::system::error_code& ec ) {
				on_connected( ec != 0 ) ;
			});

			return true ;
		} catch( boost::system::system_error& e ) {
			elog( "tcp_client::connect('%s', %d) failed. exception='%s(%d)'", host.c_str(), port, e.what(), e.code() ) ;
		}

		return false ;
	} ;

	void close( void ) {
		m_work.reset( nullptr ) ;
		m_thread->join() ;
		m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_both ) ;
		m_socket.close() ;
	} ;

	bool send( const std::string msg ) {
		if ( !connected() ) return false ;

		boost::asio::async_write( m_socket, boost::asio::buffer( msg ), 
			[this](const boost::system::error_code& ec, std::size_t bytes_transfered ) {
				if ( ec != 0 ) {
				}
		});

		return false ;
	};

	// is connected?
	bool connected( void ) {
		return m_connected.load() ;
	};

protected:

	bool send( void ) {
		return false ;
	};

	bool receive( void ) {
		return false ;
	};

	virtual void on_connected( bool connected ) {
		m_connected.store( connected ) ;
	} ;

	virtual void on_disconnected( void ) {
		m_connected.store( false ) ;
	} ;

	virtual void on_received( std::string msg ) {
	} ;


	int onThreadMain( ThreadContext pContext ) 
	{
		// io_service
		m_ios.run() ;

		return 0 ;
	};

protected:
	boost::asio::io_service m_ios ;
	boost::asio::ip::tcp::socket m_socket;
	std::unique_ptr< boost::asio::io_service::work > m_work ;
	std::unique_ptr< std::thread > m_thread ;
	std::atomic<bool> m_connected ;
};


}}; // namespace bryllite::net
*/


// program entry
int main( int argc, char** argv ) 
{
	log( "Hello, Bryllite! %s", bryllite::version() ) ;



	return 0 ;
}
