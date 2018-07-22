#pragma once

// peer server callback interface
class IPeerServer
{
public:
	// peer session callback
	virtual int onPeerSessionConnected( CPeerSession* peer_session, bool connected ) = 0;
	virtual int onPeerSessionDisconnected( CPeer* peer, int reason ) = 0;
	virtual int onPeerSessionWrite( CPeer* peer, message* msg ) = 0;
//	virtual int onPeerSessionMessage( CPeer* peer, message* msg ) = 0;

	// peer client callback
	virtual int onPeerClientConnected( CPeer* peer, bool connected ) = 0;
	virtual int onPeerClientDisconnected( CPeer* peer, int reason ) = 0;
	virtual int onPeerClientWrite( CPeer* peer, message* msg ) = 0;
//	virtual int onPeerClientMessage( CPeer* peer, message* msg ) = 0;

	virtual int onPeerMessage( CPeer* peer, message* msg ) = 0;
};




// peer server
class CPeerServer : public IPeerSession, public IPeerClient
{
	using tcp = boost::asio::ip::tcp;

protected:
	boost::asio::io_service& _ios;		// io_service ref
	tcp::acceptor _acceptor;			// acceptor
	tcp::endpoint _endpoint;			// peer server endpoint
	PeerData _peer_host;

	// IPeerServer callback interface
	IPeerServer* _peer_server_callback;

	// peers
	std::vector< std::unique_ptr< CPeer > > _peers;

	// peer sessions
	std::vector< std::unique_ptr< CPeerSession > > _sessions;
	CPeerSession* _new_session;


public:
	// ctor
	CPeerServer( boost::asio::io_service& ios, IPeerServer* peer_server_callback );

	// local node_id
	NodeID node_id( void );

	// start/stop server
	bool start_server( NodeID peer_id, std::string host, unsigned short port );
	bool start_server( PeerData host_data );
	bool stop_server( void );

	// add peer
	bool append_peer( NodeID peer_id, std::string host, unsigned short port );
	bool append_peer( PeerData peer_data );

	// find peer
	CPeer* find_peer( NodeID peer_id );

	size_t all_peer_count( void );
	size_t connected_peer_count( void );
	size_t disconnected_peer_count( void );

	// connect to peer
	size_t connect_all_peers( void );
	bool connect_peer( CPeer* peer );

	bool sendto( NodeID peer_id, message* msg );
	bool sendto( CPeer* peer, message* msg );
	size_t sendall( message* msg );

	PeerData& peer_host( void );

protected:
	// session start/stop
	bool session_start( CPeerSession* session );
	bool session_stop( CPeerSession* session );


protected:
	// accept client socket
	bool do_accept( void );
	// accept handler
	void handle_accept( CPeerSession* new_session, const boost::system::error_code& error );

public:
	// IPeerSession callback 
	int onPeerSessionConnected( CPeerSession* peer );
	int onPeerSessionDisconnected( CPeerSession* peer, int reason );
	int onPeerSessionWrite( CPeerSession* peer, message* msg );
	int onPeerSessionMessage( CPeerSession* peer, message* msg );

	// IPeerClient callback
	int onPeerClientConnected( CPeerClient* peer, bool connected );
	int onPeerClientDisconnected( CPeerClient* peer, int reason );
	int onPeerClientWrite( CPeerClient* peer, message* msg );
	int onPeerClientMessage( CPeerClient* peer, message* msg );

protected:
	int onPeerMessagePeerId( CPeerSession* peer_session, node_message_peer_id* msg );
};
