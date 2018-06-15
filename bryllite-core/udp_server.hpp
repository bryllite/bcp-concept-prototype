#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "message.inl"


// namespace bryllite::net
namespace bryllite { namespace net {

// udp server class
class udp_server
{
	using udp = boost::asio::ip::udp;

protected:
	boost::asio::io_service _ios;									// io_service
	udp::socket _socket;											// socket
	boost::asio::streambuf _read_buffer, _write_buffer;				// read/write buffer
	udp::endpoint _local_endpoint, _remote_endpoint;				// local/remote endpoint
	std::vector< std::unique_ptr< std::thread > > _io_threads;		// threads for io_service

public:
	udp_server() : _socket( _ios ) {
	};

	bool udp_start( unsigned short port, unsigned short io_thread_cnt = 1 );
	bool udp_stop( void );
	bool udp_sendto( const udp::endpoint& endPoint, message* msg );
	bool udp_sendto( const udp::endpoint& endPoint, const char* data, size_t size );

protected:
	virtual void onUdpMessage( udp::endpoint endPoint, message* msg ) {} ;
	virtual void onUdpSend( udp::endpoint endPoint, message* msg ) {};

private:
	bool do_udp_receive( void );
	void _on_udp_receive_from( const boost::system::error_code& error, size_t bytes_transferred );
	void _on_udp_send_to( const udp::endpoint& endPoint, const boost::system::error_code& error, size_t bytes_transferred );
};

}}; //namespace bryllite::net