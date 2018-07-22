#pragma once

enum { invalid_peer_id = -1 };

typedef int NodeID;

// peer data struct ( peer_id, host, port )
struct PeerData
{
public:
	NodeID _peer_id;			// peer_id
	std::string _host;			// host ( ip )
	unsigned short _port;		// port

public:
	// ctor
	PeerData();
	PeerData( NodeID peer_id, std::string host, unsigned short port );

	// accessor
	NodeID peer_id( void );
	std::string host( void );
	unsigned short port( void );

	// peer data to string
	std::string to_string( void );
};
