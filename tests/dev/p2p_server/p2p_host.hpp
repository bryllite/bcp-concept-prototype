#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

#include "byte_stream.hpp"
#include "p2p_session.hpp"
#include "peer.hpp"


class p2p_host
{
	using tcp = boost::asio::ip::tcp;
protected:
	boost::asio::io_service _ios;								// io_service
	tcp::acceptor _acceptor;									// acceptor
	tcp::endpoint _local_endpoint;								// listen endpoint
	p2p_session* _accept_session;

	std::vector< std::shared_ptr< p2p_session > > _sessions;	// sessions
	std::map< std::string, std::unique_ptr<CPeer> > _peers;		// peers
	std::vector< std::unique_ptr< std::thread > > _io_threads;	// io threads

	peer_info _me;

	bryllite::ntp_timer _ntp;

public:


protected:

};

