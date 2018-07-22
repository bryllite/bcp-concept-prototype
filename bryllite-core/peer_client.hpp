#pragma once

class CPeerClient;

// CPeerClient callback interface
class IPeerClient
{
public:
	virtual int onPeerClientConnected( CPeerClient* peer, bool connected ) = 0;
	virtual int onPeerClientDisconnected( CPeerClient* peer, int reason ) = 0;
	virtual int onPeerClientWrite( CPeerClient* peer, message* msg ) = 0;
	virtual int onPeerClientMessage( CPeerClient* peer, message* msg ) = 0;
};

class CPeer;

// peer client class
class CPeerClient
{
protected:
	enum { max_buffer_size = max_message_length + 1 };
	using tcp = boost::asio::ip::tcp;

protected:
	CPeer& _peer;
	boost::asio::io_service& _ios;			// io_service
	tcp::socket _socket;
	tcp::endpoint _endpoint;
	bool _connected;

	IPeerClient* _peer_client_callback;		// IPeerClient callback interface

	socket_buffer< 16, max_buffer_size > _read_buffer;
	socket_buffer< 8, max_buffer_size > _write_buffer;

public:
	CPeerClient( CPeer& peer, boost::asio::io_service& ios );
	CPeerClient( CPeer& peer, boost::asio::io_service& ios, IPeerClient* peer_client_callback );

	// get socket
	tcp::socket& socket( void )
	{
		return _socket;
	};

	// set peer client callback
	void set_peer_client_callback( IPeerClient* peer_client_callback );

	// is connected?
	bool connected( void );

	// get peer_id
	int peer_id( void );

	// connect to peer
	bool connect_to( PeerData peer_data );
	bool connect_to( const char* host, unsigned short port );
	bool connect_to( std::string host, unsigned short port );

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
