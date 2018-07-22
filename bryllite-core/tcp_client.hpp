#if !defined( __BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_CLIENT_HPP__ )
#define __BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_CLIENT_HPP__

/*
// namespace bryllite::net
namespace bryllite { namespace net {

// async tcp client class
// io_service.run() required outside the class
class tcp_client
{
	using tcp = boost::asio::ip::tcp;
	enum { max_buffer_cnt = 16, max_buffer_size = 4096 };

protected:
	boost::asio::io_service _ios;
	tcp::socket _socket;
	tcp::endpoint _remote_endpoint;
	socket_buffer<max_buffer_cnt, max_buffer_size> _read_buffer;
	socket_buffer<max_buffer_cnt, max_buffer_size> _write_buffer;
	std::atomic<bool> _connected;

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
	void handle_write( const boost::system::error_code& error, size_t bytes_transferred, message* msg );

	// read header handler 
	void handle_read_header( const boost::system::error_code& error, size_t bytes_transferred, message* header );

	// read body handler
	void handle_read_body( const boost::system::error_code& error, size_t bytes_transferred, message* header );

}; // tcp_client


}}; // namespace bryllite::net


*/


class CTcpClient;

// CTcpClient callback interface
class ITcpClient
{
public:
	virtual int onTcpClientConnected( CTcpClient* client, bool connected ) = 0;
	virtual int onTcpClientDisconnected( CTcpClient* client, int reason ) = 0;
	virtual int onTcpClientWrite( CTcpClient* client, message* msg, size_t bytes_transferred ) = 0;
	virtual int onTcpClientMessage( CTcpClient* client, message* msg ) = 0;
};

// tcp client class
class CTcpClient
{
protected:
	enum { max_buffer_size = max_message_length + 1 };
	using tcp = boost::asio::ip::tcp;

protected:
	boost::asio::io_service& _ios;			// io_service
	tcp::socket _socket;
	tcp::endpoint _endpoint;
	bool _connected;

	ITcpClient& _tcp_client_callback;		// IPeerClient callback interface

	socket_buffer< 16, max_buffer_size > _read_buffer;
	socket_buffer< 8, max_buffer_size > _write_buffer;

public:
	CTcpClient( boost::asio::io_service& ios, ITcpClient& tcp_client_callback );

	// get socket
	tcp::socket& socket( void )
	{
		return _socket;
	};

	// is connected?
	bool connected( void );

	// connect to peer
	bool connect( const char* host, unsigned short port );
	bool connect( std::string host, unsigned short port );
	bool connect( void );
	bool connect( tcp::endpoint ep );

	// close connection
	void close( int reason = 0 );

	// write data/message
	bool write( const byte* data, size_t len );
	bool write( message* msg );

protected:
	// wait for recv complete
	bool do_read( void );

	// connect handler
	void handle_connect( const boost::system::error_code& error );	

	// write handler
	void handle_write( const boost::system::error_code& error, size_t bytes_transferred, message* msg );

	// read header handler
	void handle_read_header( const boost::system::error_code& error, size_t bytes_transferred );

	// read body handler
	void handle_read_body( const boost::system::error_code& error, size_t bytes_transferred, message* header );
};


#endif //__BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_CLIENT_HPP__
