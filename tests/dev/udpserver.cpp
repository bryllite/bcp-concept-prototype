#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

/*
// udp echo server
class udp_server
{
	using udp = boost::asio::ip::udp;
protected:
	udp::socket _socket ;
	udp::endpoint _sender ;
	enum { max_buffer_size = 4096 };
	char _buffer[ max_buffer_size + 1 ];

public:
	udp_server( boost::asio::io_context& ioc, unsigned short port ) : _socket( ioc, udp::endpoint( udp::v4(), port ) ) {
		do_receive();
	};

	void do_receive( void ) {
		_socket.async_receive_from( boost::asio::buffer(_buffer, max_buffer_size), _sender, [this](boost::system::error_code ec, size_t bytes_transferred) {
			if ( !ec && bytes_transferred > 0 ) {
				do_send( bytes_transferred ) ;
			} else {
				do_receive();
			}
		});
	};

	void do_send( size_t bytes_transferred ) {
		_socket.async_send_to( boost::asio::buffer( _buffer, bytes_transferred ), _sender, [this](boost::system::error_code error, size_t bytes_transferred ) {
			do_receive();
		});
	};
};
*/


class _buffer 
{
protected:
	enum { max_buffer_cnt = 64, max_buffer_size = 4096 };
	char _readBuffer[max_buffer_cnt + 1][max_buffer_size + 1];
	char _writeBuffer[max_buffer_cnt + 1][max_buffer_size + 1];
	int _readBufferPos, _writeBufferPos;

	char* readBuffer( void ) {
		_readBufferPos = ( ( ++_readBufferPos ) % max_buffer_cnt );
		return _readBuffer[_readBufferPos];
	};

	char* writeBuffer( void ) {
		_writeBufferPos = ( ( ++_writeBufferPos ) % max_buffer_cnt );
		return _writeBuffer[_writeBufferPos];
	};

public:
	_buffer() : _readBufferPos(-1), _writeBufferPos(-1) {
	};
};


// udp echo server
class udp_server : public _buffer
{
	using udp = boost::asio::ip::udp;
protected:
	boost::asio::io_service _ios;
	udp::socket _socket;
	udp::endpoint _remote_endpoint;
	std::unique_ptr< std::thread > _io_thread ;

public:
	udp_server( unsigned short port ) : _socket( _ios, udp::endpoint( udp::v4(), port ) ) {
		do_receive();

		// io thread
		_io_thread.reset( new std::thread( boost::bind( &boost::asio::io_service::run, &_ios ) ) );
	};

protected:

	void do_receive( void ) 
	{
		char* recv_buffer = readBuffer();

		_socket.async_receive_from( boost::asio::buffer( recv_buffer, max_buffer_size ), _remote_endpoint, boost::bind( &udp_server::handle_receive_from, this, recv_buffer, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, _remote_endpoint ) );
	};

	void handle_receive_from( char* recv_buffer, const boost::system::error_code& error, size_t bytes_transferred, udp::endpoint& endpoint ) 
	{
		if ( !error && bytes_transferred > 0 ) {
			log( "udp_server::handle_receive_from() >> %d bytes_transferred", bytes_transferred );
			log( "_remote_endpoint=%s:%d", _remote_endpoint.address().to_string().c_str(), _remote_endpoint.port() );
			log( "endpoint=%s:%d", endpoint.address().to_string().c_str(), endpoint.port() );

			// reply message
			send_to( _remote_endpoint, recv_buffer, bytes_transferred );

		} else {
			wlog( "udp_server::handle_receive_from() >> error=(%d)%s", error.value(), error.message() ) ;
		}

		do_receive();
	};

	void send_to( udp::endpoint endpoint, const char* data, int len ) {

		if ( len > max_buffer_size ) {
			elog( "udp_server::send() >> buffer overflow. len=%d", len );
			return ;
		}

		// copy data to write buffer
		char* write_buffer = writeBuffer();
		memcpy( write_buffer, data, len );
		write_buffer[ len + 1 ] = 0;

		_socket.async_send_to( boost::asio::buffer( write_buffer, len ), endpoint, boost::bind( &udp_server::handle_send_to, this, endpoint, write_buffer, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
	};

	void handle_send_to( udp::endpoint endpoint, const char* write_buffer, const boost::system::error_code& error, size_t bytes_transferred ) {
		log( "udp_server::handle_send_to() >> error=%d, bytes_transferred=%d", error.value(), bytes_transferred );

	};

};




// udp echo server
int main( int argc, char** argv ) 
{
	log( "Hello, Bryllite! %s", bryllite::version() ) ;

	unsigned short port = ( argc >= 2 ) ? atoi(argv[1]): 9000 ;

	try 
	{
		udp_server s( port );

		log( "udp_server started on port=%d", port );

		while( true ) {
			bryllite::sleep_for(0);
		};
	}
	catch( std::exception& e )
	{
		elog( "Exception: e=%s", e.what() ) ;
	}
	catch( ... )
	{
		elog( "Unknown exception" ) ;
	}

	return 0 ;
}
