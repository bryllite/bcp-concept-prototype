#if !defined( __BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_SERVER_HPP__ )
#define __BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_SERVER_HPP__

/*
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
	socket_buffer< max_buffer_cnt, max_buffer_size > _read_buffer;
	socket_buffer< max_buffer_cnt, max_buffer_size > _write_buffer;

	std::atomic<bool> _active;
	time_t _closed ;

public:
	tcp_session( boost::asio::io_service& ios );
	virtual ~tcp_session();

	bool active( void );
	void active( bool active_ );

	tcp::socket& socket( void );

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
	tcp_session* _new_session;
	std::vector< std::unique_ptr< std::thread > > _io_threads;	// io threads

public:
	tcp_server();

	bool tcp_start( unsigned short port, int io_thread_cnt = 1 ); 
	bool tcp_stop( void );

	bool start_server( unsigned short port, int io_thread_cnt = 1 ) {
		return tcp_start( port, io_thread_cnt );
	};

	bool stop_server( void ) {
		return tcp_stop();
	};



	// close_session
	void close_session( tcp_session* session ); 

	// remove_session if expired
	void remove_session_if( time_t expired );

	// sending message
	void tcp_send( tcp_session* session, message* msg ); 
	void tcp_send( tcp_session* session, const char* data, size_t len );

	// write message to all client ( except for except_session )
	void tcp_broadcast( message* msg, tcp_session* except_session = nullptr );

	void sendto( tcp_session* session, message* msg ) {
		tcp_send( session, msg );
	};

	void sendto( tcp_session* session, const char* data, size_t len ) {
		tcp_send( session, data, len );
	};

	void sendall( message* msg ) {
		tcp_broadcast( msg );
	};


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
	void session_read_payload( tcp_session* session, message* header );

	// read header procedure
	void handle_session_read_header( tcp_session* session, const boost::system::error_code& error, size_t bytes_transferred, message* header );

	// read payload procedure
	void handle_session_read_payload( tcp_session* session, const boost::system::error_code& error, size_t bytes_transferred, message* header );

	// write handler
	void handle_session_write( tcp_session* session, const boost::system::error_code& error, size_t bytes_transferred, message* header );


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
*/


class CTcpSession;

// tcp session callback interface
class ITcpSession 
{
public:
	virtual int onTcpSessionConnected( CTcpSession* session, bool connected ) = 0;
	virtual int onTcpSessionDisconnected( CTcpSession* session, int reason ) = 0;
	virtual int onTcpSessionWrite( CTcpSession* session, message* msg, size_t bytes_transferred ) = 0;
	virtual int onTcpSessionMessage( CTcpSession* session, message* msg ) = 0;
};

// tcp session class
class CTcpSession
{
	using tcp = boost::asio::ip::tcp;
	friend class CTcpServer;

	enum { max_buffer_cnt = 16, max_buffer_size = max_message_length };

protected:
	CTcpServer& _tcp_server;		// tcp server ref
	boost::asio::io_service& _ios;	// io_service ref
	tcp::socket _socket;			// socket

	// read/write buffer
	socket_buffer< max_buffer_cnt, max_buffer_size > _read_buffer;
	socket_buffer< max_buffer_cnt, max_buffer_size > _write_buffer;

	// is connected?
	bool _connected;

	// tcp session callback interface
	ITcpSession* _tcp_session_callback;

public:
	CTcpSession( CTcpServer& tcp_server, boost::asio::io_service& ios, ITcpSession* tcp_session_callback );
	virtual ~CTcpSession();

	tcp::socket& socket( void );

	// is connected?
	bool connected( void );

	// close connection
	void close( int reason = 0 );

	// write message
	bool write( byte* data, size_t len );
	bool write( message* msg );

	// session start/stop
	bool start( void );
	bool stop( void );

	// remote addr
	std::string ip( void );
	unsigned short port( void );

protected:
	// read
	bool do_read( void );

	// read header handler
	void handle_read_header( const boost::system::error_code& error, size_t bytes_transferred );

	// read body handler
	void handle_read_body( const boost::system::error_code& error, size_t bytes_transferred, message* header );

	// write handler
	void handle_write( const boost::system::error_code& error, size_t bytes_transferred, message* msg );

};


class ITcpServer
{
public:
	virtual int onTcpServerAccept( CTcpSession* session ) = 0;
	virtual int onTcpServerDisconnect( CTcpSession* session, int reason ) = 0;
	virtual int onTcpServerWrite( CTcpSession* session, message* msg, size_t bytes_trasnferred ) = 0;
	virtual int onTcpServerMessage( CTcpSession* session, message* msg ) = 0;
};


// tcp server ( using callback interface )
class CTcpServer : public ITcpSession
{
	using tcp = boost::asio::ip::tcp;
protected:

	boost::asio::io_service& _ios;
	tcp::acceptor _acceptor;
	tcp::endpoint _local_endpoint;

	bryllite::lockable _lock;

	// tcp server callback interface
	ITcpServer& _tcp_server_callback;

	// sessions
	std::vector< std::unique_ptr< CTcpSession > > _sessions;
	CTcpSession* _new_session;

	bool _stop;

public:
	// ctor
	CTcpServer( boost::asio::io_service& ios, ITcpServer& tcp_server_callback );

	// start/stop server
	bool start_server( unsigned short port );
	bool stop_server( void );

	// send message
	bool sendto( CTcpSession* session, message* msg );
	size_t sendall( message* msg );

	size_t session_count( void );

protected:
	// allocate session
	virtual CTcpSession* new_session( void );

protected:
	// accept client socket
	bool accept( void );
	// accept handler
	void handle_accept( CTcpSession* new_session, const boost::system::error_code& error );


public:
	// tcp session callback
	int onTcpSessionConnected( CTcpSession* session, bool connected );
	int onTcpSessionDisconnected( CTcpSession* session, int reason );
	int onTcpSessionWrite( CTcpSession* session, message* msg, size_t bytes_transferred );
	int onTcpSessionMessage( CTcpSession* session, message* msg );
};


#endif //__BRYLLITE_PLATFORM_BRYLLITE_CORE_LIB_TCP_SERVER_HPP__