#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>



// udp echo client
int main( int argc, char** argv ) 
{
	using udp = boost::asio::ip::udp;

	log( "Hello, Bryllite! %s", bryllite::version() ) ;

	std::string host = ( argc >=2 ) ? argv[1] : "127.0.0.1" ;
	unsigned short port = ( argc >= 3 ) ? atoi(argv[2]) : 9000 ;

	enum { max_buffer_len = 4096 };
	char _request[ max_buffer_len + 1 ];
	char _reply[ max_buffer_len + 1 ];

	try 
	{
		boost::asio::io_context ioc ;
		udp::endpoint _endpoint( boost::asio::ip::address::from_string(host), port );
		udp::socket _socket( ioc, udp::endpoint( udp::v4(), 0 ) );

		bryllite::timer ts ;
		while( ts.laptime() <= 60000 ) {
			std::cout << "Enter message to send: " ;
			std::cin.getline( _request, max_buffer_len );
			size_t request_len = strlen( _request );

			_socket.send_to( boost::asio::buffer( _request, request_len ), _endpoint );
			udp::endpoint _sender;
			size_t reply_length = _socket.receive_from( boost::asio::buffer( _reply, max_buffer_len ), _sender );

			std::cout << "Reply: " ;
			std::cout.write( _reply, reply_length );
			std::cout << std::endl ;
		}
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
