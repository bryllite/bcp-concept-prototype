#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

#include "byte_stream.hpp"
#include "p2p_client.hpp"
#include "p2p_session.hpp"
#include "peer.hpp"



// p2p server
class p2p_server : public bryllite::lockable
{
	using tcp = boost::asio::ip::tcp;
protected:
	boost::asio::io_service _ios;								// io_service
	tcp::acceptor _acceptor;									// acceptor
	tcp::endpoint _local_endpoint;								// listen endpoint
	p2p_session* _accept_session;

	std::vector< std::shared_ptr< p2p_session > > _sessions;	// sessions
	std::vector< std::unique_ptr< std::thread > > _io_threads;	// io threads

	std::map< std::string, std::unique_ptr<CPeer> > _peers;

	peer_info _me;

	bryllite::ntp_timer _ntp;


public:
	p2p_server();

	bool start_server( std::string node_id, std::string host, unsigned short port, int io_thread_cnt = 1 ); 
	bool stop_server( void );

	// close_session
	void close_session( p2p_session* session ); 

	// sending message
	void sendto( p2p_session* session, message* msg ); 
	void sendto( p2p_session* session, byte* data, size_t len );

	// write message to all client ( except for except_session )
	void sendall( message* msg );
	void sendall( byte* data, size_t len );

public:

	// find peer by peer_id
	CPeer* find_peer( std::string peer_id );

	// append peer
	bool append_peer( std::string peer_id, std::string host, unsigned short port );

	// connect to peers
	bool connect_to_peers( void );
	bool connect_to_peer( CPeer* peer );

protected:

	// remove_session if expired
	void remove_session_if( time_t expired );

	// new session factory
	virtual p2p_session* new_session( void );

	bool accept( void );

	// accept procedure
	void handle_accept( p2p_session* session, const boost::system::error_code& error );

	// session start with read header
	void session_start( p2p_session* session ) ;

	// read header
	void session_read_header( p2p_session* session );
	// read header procedure
	void handle_session_read_header( p2p_session* session, const boost::system::error_code& error, size_t bytes_transferred );


	// read payload
	void session_read_body( p2p_session* session, message* header );
	// read payload procedure
	void handle_session_read_body( p2p_session* session, message* header, const boost::system::error_code& error, size_t bytes_transferred );


	// new connection
	virtual void onAccept( p2p_session* session );

	// disconnected
	virtual void onDisconnected( p2p_session* session );

	// message received
	virtual void onMessage( p2p_session* session, message* msg );

};


