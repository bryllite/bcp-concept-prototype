#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

//#include "message.inl" 

const unsigned short tcp_port = 8000;
const unsigned short udp_port = 9000;

/*
// namespace bryllite::net
namespace bryllite { namespace net {

// tcp server class
class tcp_server
{
public:
	using tcp = boost::asio::ip::tcp ;

	// user session class : can be override
	class session
	{
	protected:
		enum { max_buffer_size = 4096 };
		tcp::socket _socket;
		char _buffer[ max_buffer_size + 1 ];

	public:
		session( boost::asio::io_service& ios ) : _socket( ios ) {
		};

		tcp::socket& socket() { return _socket ; };

		virtual void start( void ) {
			// read header
			read_header() ;
		};

	private:
		// read message header
		void read_header( void ) {
			memset( _buffer, 0, sizeof( _buffer ) );

			// read header
			boost::asio::async_read( _socket, boost::asio::buffer( _buffer, max_buffer_size ), boost::asio::transfer_exactly( sizeof(message_header) ),
									 boost::bind( &session::handle_read_header, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
		};

		// ready message header first
		void handle_read_header( const boost::system::error_code& error, size_t bytes_transferred ) {
			if( error ) {
				wlog( "handle_read_header() failed. error=%d, bytes_transferred=%d", error, bytes_transferred );
				delete this;
				return;
			}

			bryllite::net::message_header* header = reinterpret_cast<bryllite::net::message_header*>( _buffer );
			if( !header->valid() ) {
				wlog( "invalid message header" );
				delete this;
				return;
			}

			// ready body
			boost::asio::async_read( _socket, boost::asio::buffer( _buffer + sizeof( bryllite::net::message_header ), max_buffer_size - sizeof(message_header) ), 
				boost::asio::transfer_exactly( header->size() - sizeof(message_header) ), 
				boost::bind( &session::handle_read_body, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
		};

		// read message body
		void handle_read_body( const boost::system::error_code& error, size_t bytes_transferred ) {
			if( error ) {
				wlog( "handle_read_body() failed. error=%d, bytes_transferred=%d", error, bytes_transferred );
				delete this;
				return;
			}

			bryllite::net::message* msg = reinterpret_cast<bryllite::net::message*>( _buffer );
			if( !msg->valid() ) {
				wlog( "handle_read_body(): invalid message" );
				delete this;
				return;
			}

			log( "new message: %s length=%d", msg->payload(), msg->size() );

			// disconnect request
			if ( msg->msgid() == 1 ) {
				log( "disconnect request" ) ;
				delete this ;
				return ;
			}

			// reply message
			boost::asio::async_write( _socket, boost::asio::buffer( ( char* )msg->buffer(), msg->size() ), boost::bind( &session::handle_write, this, boost::asio::placeholders::error ) );
		};

		void handle_write( const boost::system::error_code& error ) {
			if( !error ) {
				read_header();
			}
			else {
				elog( "handle_write() failed. error=%d", error );
				delete this;
			}
		};
	};	// class session

public:
	tcp_server( boost::asio::io_service& ios, unsigned short port )
		: _ios( ios ), _acceptor( ios, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), port ) ) {

		start_accept();
	};

private:
	void start_accept( void ) {
		session* new_session = new session( _ios );
		log( "start_accept() started" );
		_acceptor.async_accept( new_session->socket(), boost::bind( &tcp_server::handle_accept, this, new_session, boost::asio::placeholders::error ) );
	};

	void handle_accept( session* new_session, const boost::system::error_code& error ) {
		if( !error ) {
			log( "new connection accepted" );
			new_session->start();
		}
		else {
			elog( "handle_accept() failed. error=%d", error );
			delete new_session;
		}

		start_accept();
	};

	boost::asio::io_service& _ios;
	boost::asio::ip::tcp::acceptor _acceptor;

}; // class tcp_server


}}; // namespace bryllite::net
*/


// tcp/udp echo server
int main( int argc, char** argv ) 
{
	log( "Hello, Bryllite! %s", bryllite::version() ) ;

	std::thread t1( []() {
		boost::asio::io_service ios ;
		bryllite::net::tcp_server server( ios, tcp_port ) ; 

		log( "io_service.run() started" ) ;
		ios.run() ;
	});
	log( "waiting for tcp_server..." ) ;
	t1.join() ;

	return 0 ;
}
