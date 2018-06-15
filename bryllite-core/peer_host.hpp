#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "message.inl"


// namespace bryllite::net
namespace bryllite { namespace net {

// peer session
class peer_session
{
	using udp = boost::asio::ip::udp;
protected:
	udp::endpoint _endpoint;
	time_t _heartbeat;

public:
	peer_session();
	peer_session( udp::endpoint endpoint );
	peer_session( const char* host, unsigned short port );

	void touch( void );
	bool alive( time_t time );

	udp::endpoint ep(void);

	// unique id ( "127.0.0.1:8888" )
	std::string id( void );
	boost::asio::ip::address address( void );
	unsigned short port( void );
};


// peer to peer host class
class peer_host
{
	using udp = boost::asio::ip::udp;

protected:
	boost::asio::io_service _ios;									// io_service
	udp::socket _socket;											// socket
	boost::asio::streambuf _read_buffer, _write_buffer;				// read/write buffer
	udp::endpoint _local_endpoint, _remote_endpoint;				// local/remote endpoint
	std::vector< std::unique_ptr< std::thread > > _io_threads;		// threads for io_service
	std::vector< std::shared_ptr<peer_session> > _peers;
	bryllite::lockable _peer_lock;									// peer list lock

public:
	peer_host() : _socket( _ios ) {
	};

	// peer host start & stop
	bool peer_start( unsigned short port, unsigned short io_thread_cnt = 1 );
	bool peer_stop( void );

	// peer send message
	bool peer_sendto( const udp::endpoint& endPoint, message* msg );
	bool peer_sendto( const udp::endpoint& endPoint, const char* data, size_t size );
	bool peer_sendto( peer_session* session, message* msg );
	bool peer_sendto( peer_session* session, const char* data, size_t size );

	// peer broadcast message
	bool peer_sendall( message* msg );
	bool peer_sendall( const char* data, size_t size );

	// append authorized peer
	bool peer_append( const char* host, unsigned short port );
	bool peer_append( udp::endpoint peer );

	// get peer session
	peer_session* peer_find( udp::endpoint peer );

	// is authorized peer?
	bool is_authorized_peer( udp::endpoint peer );

	// peer count
	size_t peer_count( void );

protected:
	virtual peer_session* new_peer_session( udp::endpoint peer );

protected:
	// message callback
	virtual void onPeerMessage( udp::endpoint peer, message* msg ) {} ;	
	virtual void onPeerWrite( udp::endpoint peer, message* msg ) {};

private:
	// io handler
	bool do_peer_receive( void );
	void _on_peer_receive_from( const boost::system::error_code& error, size_t bytes_transferred );
	void _on_peer_send_to( const udp::endpoint& endPoint, const boost::system::error_code& error, size_t bytes_transferred );
};

}}; //namespace bryllite::net