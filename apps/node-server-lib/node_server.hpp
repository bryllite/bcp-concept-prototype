#pragma once

class app_config;

// node server class
class CNodeServer final : public IPeerServer, public IBridgeServer, public IUserServer, public ICallbackTimer, public IBcpServerCallback
{
	using callback_timer = bryllite::callback_timer;

private:
	// lock object
	bryllite::lockable _lock;

	boost::asio::io_service _io_service;
	std::vector< std::unique_ptr< std::thread > > _io_threads;

	// config structure
	app_config& _app_config;

	// node to node peer server
	CPeerServer _peer_server;

	// node to bridge server
	CBridgeServer _bridge_server;

	// node to user server
	CUserServer _user_server;

	// block chain
	CBlockChain _block_chain;

	// transaction memory pool
	CTransactions _tx_mem_pool;

	// node account
	CAccount _node_account;

	// callback timer
	callback_timer _callback_timer;

	// bcp class
	CBcpServer _bcp_server;

	// pingid
	size_t _ping_id;

public:
	// ctor
	CNodeServer(app_config& appConfig);

	// node id
	int node_id( void );

	// start/stop server
	bool start_server( size_t io_worker_thread = 1 );
	bool stop_server( void );

	// connect to peers
	bool connect_peers( void );

	// append peer
	bool append_peer( PeerData peerData );

	// peer count ( connected, not connected )
	size_t all_peer_count( void );
	size_t connected_peer_count( void );
	size_t disconnected_peer_count( void );

	// main processor
	int update( void );

	// send message to peer
	size_t peer_sendall( message* msg );
	bool peer_sendto( CPeer* peer, message* msg );

	// send message to bridge server
	size_t sendto_bridge( message* msg );

protected:
	// create block data
	bool create_block_for( size_t block_index, CBlock& block );

	// add tx into block from tx.mem.pool
	bool add_mem_pool_tx( CBlock& block );

	// append tx to memory pool
	bool append_mem_pool_tx( CTransaction tx );

private:

	// io worker thread run
	bool io_worker_run( size_t io_worker_count );

	// timer id
	enum {
		timer_id_ping = 100,
		timer_id_connect_peers,
	};

	// callback timer handler
	void onTimeOut( timer_id id, void* pContext );
	void onTimeOutPing( void* pContext );
	void onTimeOutConnectPeers( void* pContext );

	// peer server handler
public:
	// IPeerServer callback
	int onPeerSessionConnected( CPeerSession* peer_session, bool connected );
	int onPeerSessionDisconnected( CPeer* peer, int reason );
	int onPeerSessionWrite( CPeer* peer, message* msg );
	int onPeerClientConnected( CPeer* peer, bool connected );
	int onPeerClientDisconnected( CPeer* peer, int reason );
	int onPeerClientWrite( CPeer* peer, message* msg );
	int onPeerMessage( CPeer* sender, message* msg );
protected:
	// peer message handler
	int onPeerMessagePeerId( CPeer* sender, node_message_peer_id* msg );
	int onPeerMessageBlockNotify( CPeer* sender, node_message_block_notify* msg );
	int onPeerMessageBlockReq( CPeer* sender, node_message_block_req* msg );
	int onPeerMessageNewRound( CPeer* sender, node_message_new_round* msg );
	int onPeerMessagePropose( CPeer* sender, node_message_propose* msg );
	int onPeerMessageVote( CPeer* sender, node_message_vote* msg );
	int onPeerMessageVerifyBlock( CPeer* sender, node_message_verify_block* msg );
	int onPeerMessageCommit( CPeer* sender, node_message_commit* msg );
	int onPeerMessageTransactionsNotify(CPeer* sender, message_transactions_notify* msg);


	// node-bridge server handler
public:
	int onBridgeAccept( CTcpSession* session );
	int onBridgeDisconnected( CTcpSession* session, int reason );
	int onBridgeWrite( CTcpSession* session, message* msg, size_t bytes_transferred );
	int onBridgeMessage( CTcpSession* session, message* msg );

	// node-user server handler
public:
	int onUserAccept( CTcpSession* session );
	int onUserDisconnected( CTcpSession* session, int reason );
	int onUserWrite( CTcpSession* session, message* msg, size_t bytes_transferred );
	int onUserMessage( CTcpSession* session, message* msg );
protected:
	int onUserMessageHeaderSignAck( CTcpSession* session, message_header_sign_ack* msg );
	int onUserMessageBalanceReq( CTcpSession* session, message_balance_req* msg );

	// CBcpServer handler
public:
	size_t qualify_votes_count( void );
	int onBcpNewRoundReady( size_t prevRoundIdx );
	int onBcpNewRoundStart( size_t roundIdx );
	int onBcpProposeStart( size_t roundIdx );
	int onBcpVoteStart( size_t roundIdx );
	int onBcpCommitStart( size_t roundIdx, CBlockHeader vote_header );
	bool onBcpVerifyBlock( size_t roundIdx, CBlock verify_block );
	int onBcpNewBlock( size_t roundIdx, CBlock newBlock );
	int onBcpTimeout( size_t roundIdx );

};
