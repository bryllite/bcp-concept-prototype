#pragma once


// peer class
class CPeer
{
protected:
	boost::asio::io_service& _ios;

	PeerData _peer_data;			// peer data

	CPeerSession* _peer_session;	// peer_session
	CPeerClient _peer_client;		// peer_client

	uint256 _auth_code;				// authentication code

public:
	// ctor
	CPeer( boost::asio::io_service& ios );
	CPeer( boost::asio::io_service& ios, PeerData peer_data );
	CPeer( boost::asio::io_service& ios, NodeID peer_id, std::string host, unsigned short port );

	NodeID peer_id( void );
	std::string host( void );
	unsigned short port( void );

	// connect to peer
	bool connect_peer( IPeerClient* peer_client_callback );

	// client/session connected?
	bool client_connected( void );
	bool session_connected( void );
	bool connected( void );

	// sendto client
	bool send( message* msg );

	// generate random auth code
	uint256 auth_code( void );

	// peer information to string
	std::string to_string( void );

public:

	// peer session
	void peer_session( CPeerSession* session );
	CPeerSession* peer_session( void );

};
