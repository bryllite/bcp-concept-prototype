#pragma once

#include <boost/asio.hpp>


// application config
struct app_config
{
	using udp = boost::asio::ip::udp;

	int _node_id;					// node id

	unsigned short _peer_port ;		// node <-> node port
	unsigned short _bridge_port ;	// node <-> bridge server port
	unsigned short _game_port ;		// node <-> game client port

	std::string _key_file;			// node account key file

	std::vector< udp::endpoint > _peers;	// node peer list

	app_config() : _peer_port(0), _bridge_port(0), _game_port(0) {
	};

	void append_peer( const char* host, unsigned short port ) {
		_peers.push_back( udp::endpoint( boost::asio::ip::address::from_string( host ), port ) );
	};

};
