#pragma once


#include <peer_server.hpp>

// node server class
class CNodeServer : public IPeerServer, public ICallbackTimer
{
	using ntp_timer = bryllite::ntp_timer;
	using callback_timer = bryllite::callback_timer;

protected:
	boost::asio::io_service _ios;								// io_service
	std::vector< std::unique_ptr<std::thread> > _io_threads;	// io threads

	PeerData _host_data;			// host data
	CPeerServer _peer_server;		// peer server

	ntp_timer _ntp;					// ntp timer
	callback_timer _callback_timer;	// callback timer

	size_t _ping_id;				// ping id

	enum 
	{
		timer_connect_peers = 100,	// reconnect all peers timer
		timer_ping,					// sending ping timer
	};


public:
	// ctor
	CNodeServer();

public:
	// start/stop server
	bool start_server( PeerData host_data, int thread_count = 1 );
	bool stop_server( void );	

	// connect to peers
	bool connect_peers( void );

	// append peer
	bool append_peer( PeerData peer_data );

	// main procedure
	int main( void );

	// node_id
	int node_id( void );

protected:
	void onTimeOut( timer_id tid, void* pContext );

public:
	// peer session callback
	int onPeerSessionConnected( CPeerSession* peer_session, bool connected );
	int onPeerSessionDisconnected( CPeer* peer, int reason );
	int onPeerSessionWrite( CPeer* peer, message* msg );
//	int onPeerSessionMessage( CPeer* peer, message* msg );

	// peer client callback
	int onPeerClientConnected( CPeer* peer, bool connected );
	int onPeerClientDisconnected( CPeer* peer, int reason );
	int onPeerClientWrite( CPeer* peer, message* msg );
//	int onPeerClientMessage( CPeer* peer, message* msg );

	// client/session message handler
	int onPeerMessage( CPeer* peer, message* msg );

protected:
	int onPeerMessageText( CPeer* peer, message_text* msg );
	int onPeerMessagePing( CPeer* peer, message_ping* msg );
	int onPeerMessagePong( CPeer* peer, message_pong* msg );

protected:
	bool ios_run( size_t thread_count = 1 );	// io_worker thread
};
