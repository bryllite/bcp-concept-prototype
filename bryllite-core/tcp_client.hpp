#if !defined( __BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_CLIENT_HPP__ )
#define __BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_CLIENT_HPP__

#include "bryllite-common.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "message.inl"

#pragma _TODO("[2018-05-20] need to replace _readBuffer, _writeBuffer to var buffers")

// namespace bryllite::net
namespace bryllite { namespace net {

// async tcp client class
// io_service.run() required outside the class
class tcp_client
{
	using tcp = boost::asio::ip::tcp;
	enum { max_buffer_cnt = 64, max_buffer_size = 4096 };

protected:
	boost::asio::io_service _ios;
	tcp::socket _socket;
	tcp::endpoint _remote_endpoint;
	char _readBuffer[max_buffer_cnt + 1][max_buffer_size + 1];
	char _writeBuffer[max_buffer_cnt + 1][max_buffer_size + 1];
	std::atomic<bool> _connected;
	int _readBufferPos, _writeBufferPos;

	std::unique_ptr< std::thread > _io_thread;	// io worker thread for io_service

public:
	tcp_client();
	tcp_client( const char* host, unsigned short port );

	bool tcp_client_start( void );
	bool tcp_client_start( const char* host, unsigned short port );
	bool tcp_client_stop( void );

	// get socket
	tcp::socket& socket( void ) {
		return _socket;
	};

	// connect
	bool connect( void );
	bool connect( const char* host, unsigned short port );

	// close connection
	void close( void ); 

	// is connected?
	bool connected( void );

	// write data
	bool write( const char* data, size_t len );

	// write message
	bool write( message* msg );

protected:
	virtual void onConnected( bool connected );
	virtual void onDisconnected( void );
	virtual void onReceived( message* msg );
	virtual void onWrited( size_t bytes_transferred );

private:
	// wait for receive completion
	void do_read( void );

	// connect handler
	void handle_connect( const boost::system::error_code& error );

	// write handler
	void handle_write( const boost::system::error_code& error, size_t bytes_transferred, int writeBufferPos );

	// read header handler 
	void handle_read_header( const boost::system::error_code& error, size_t bytes_transferred, int readBufferPos );

	// read body handler
	void handle_read_body( const boost::system::error_code& error, size_t bytes_transferred, int readBufferPos );

}; // tcp_client


}}; // namespace bryllite::net

#endif //__BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_CLIENT_HPP__
