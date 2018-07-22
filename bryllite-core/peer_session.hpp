#pragma once

class CPeerSession;

class IPeerSession
{
public:
	virtual int onPeerSessionConnected( CPeerSession* peer_session ) = 0;
	virtual int onPeerSessionDisconnected( CPeerSession* peer_session, int reason ) = 0;
	virtual int onPeerSessionWrite( CPeerSession* peer_session, message* msg ) = 0;
	virtual int onPeerSessionMessage( CPeerSession* peer_session, message* msg ) = 0;
};


// peer session class
class CPeerSession
{
	using tcp = boost::asio::ip::tcp;

	enum { max_buffer_size = max_message_length + 1 };

protected:
	boost::asio::io_service& _ios;
	tcp::socket _socket;
	bool _connected;			// connected?
	int _peer_id;				// peer id

	IPeerSession* _peer_session_callback;

	// read/write buffer
	socket_buffer<32, max_buffer_size > _read_buffer;
	socket_buffer<16, max_buffer_size > _write_buffer;

public:
	// ctor
	CPeerSession(boost::asio::io_service& ios );
	CPeerSession(boost::asio::io_service& ios, IPeerSession* peer_session_callback );

	// is connected?
	bool connected( void );

	// socket
	tcp::socket& socket( void ) {
		return _socket;
	};

	// close connection
	void close( int reason = 0 );

	// write message
	bool write( byte* data, size_t len );
	bool write( message* msg );

	// session start/stop
	bool start( void );
	bool stop( void );

	// peer_id accessor
	void peer_id( int peer_id );
	int peer_id( void );

	// peer session to string
	std::string to_string( void );

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
