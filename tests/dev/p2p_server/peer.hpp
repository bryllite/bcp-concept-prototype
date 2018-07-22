#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

#include "p2p_client.hpp"
#include "p2p_session.hpp"


class peer_info
{
protected:
	std::string _peer_id;
	std::string _host;
	unsigned short _port;

public:
	peer_info() 
	{
	};

	peer_info( std::string peer_id, std::string host, unsigned short port ) : _peer_id(peer_id), _host(host), _port(port)
	{
	};

	std::string peer_id( void )
	{
		return _peer_id;
	};

	std::string host( void )
	{
		return _host;
	};

	unsigned short port( void )
	{
		return _port;
	};

	bool operator==( const peer_info& peer )
	{
		if ( _peer_id.compare( peer._peer_id ) ) return false;
		if ( _host.compare( peer._host ) ) return false;
		if ( _port != peer._port ) return false;

		return true ;
	};

};


// peer class
class CPeer : public p2p_client
{
protected:
	peer_info _me;
	peer_info _peer;

	p2p_session* _session;		// p2p session for peer

public:
	CPeer( boost::asio::io_service& ios, peer_info peer, peer_info me ) ;

	std::string peer_id( void );

	p2p_session* session( void );

	void session( p2p_session* session );

	bool connect_peer( void );
	bool compare( peer_info peer );

protected:
	void onConnected( bool connected );
	void onDisconnected( void );
	void onReceived( message* msg );
	void onWrited( size_t bytes_transferred );
};

