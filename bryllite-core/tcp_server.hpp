#if !defined( __BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_SERVER_HPP__ )
#define __BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_SERVER_HPP__

#include "bryllite-common.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "message.inl"


#pragma _TODO("[2018-05-20] need to replace _readBuffer, _writeBuffer to var buffers")

// namespace bryllite::net
namespace bryllite { namespace net {

// tcp session class
class tcp_session
{
	using tcp = boost::asio::ip::tcp;
	friend class tcp_server;
protected:
	tcp::socket _socket;
	enum { max_buffer_cnt = 64, max_buffer_size = 4096 };
	char _read_buffer[max_buffer_cnt + 1][max_buffer_size + 1];
	char _write_buffer[max_buffer_cnt + 1][max_buffer_size + 1];
	int _read_buffer_pos, _write_buffer_pos ;
	std::atomic<bool> _active;
	time_t _closed ;

public:
	tcp_session( boost::asio::io_service& ios );
	virtual ~tcp_session();

	bool active( void );
	void active( bool active_ );

	tcp::socket& socket( void );

	// read buffer
	char* read_buffer( int read_buffer_pos );
	size_t read_buffer_size( void );
	int read_buffer_pos( void);

	// write buffer
	char* write_buffer( int write_buffer_pos );
	size_t write_buffer_size( void );
	int write_buffer_pos( void );

	void close( void );
	bool expired( time_t timeout );

}; // tcp_session

// tcp server class
class tcp_server : public lockable
{
	using tcp = boost::asio::ip::tcp;
protected:
	boost::asio::io_service _ios;		// io_service
	tcp::acceptor _acceptor;			// acceptor
	tcp::endpoint _local_endpoint;		// listen endpoint
	std::vector< std::shared_ptr< tcp_session > > _sessions;	// sessions
	std::vector< std::unique_ptr< std::thread > > _io_threads;	// io threads

public:
	tcp_server();

	bool tcp_start( unsigned short port, int io_thread_cnt = 1 ); 
	bool tcp_stop( void );

	// close_session
	void close_session( tcp_session* session ); 

	// remove_session if expired
	void remove_session_if( time_t expired );

	// sending message
	void tcp_send( tcp_session* session, message* msg ); 
	void tcp_send( tcp_session* session, const char* data, size_t len );

	// write message to all client ( except for except_session )
	void tcp_broadcast( message* msg, tcp_session* except_session = nullptr );

protected:

	// new session factory
	virtual tcp_session* new_session( void );

	bool accept( void );

	// accept procedure
	void handle_accept( tcp_session* session, const boost::system::error_code& error );

	// session start with read header
	void session_start( tcp_session* session ) {
		session_read_header( session );
	};

	// read header
	void session_read_header( tcp_session* session );

	// read payload
	void session_read_payload( tcp_session* session, message_header* header, int read_buffer_pos );

	// read header procedure
	void handle_session_read_header( tcp_session* session, const boost::system::error_code& error, size_t bytes_transferred, int read_buffer_pos );

	// read payload procedure
	void handle_session_read_payload( tcp_session* session, const boost::system::error_code& error, size_t bytes_transferred, int read_buffer_pos );

	// write handler
	void handle_session_write( tcp_session* session, const boost::system::error_code& error, size_t bytes_transferred, int write_buffer_pos );


	// new connection
	virtual void onAccept( tcp_session* session );

	// disconnected
	virtual void onTcpDisconnect( tcp_session* session );

	// message received
	virtual void onTcpMessage( tcp_session* session, message* msg );

	// write completed
	virtual void onTcpWrite( tcp_session* session, size_t bytes_transferred );

}; // tcp_server




}}; //namespace bryllite::net

#endif //__BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_SERVER_HPP__