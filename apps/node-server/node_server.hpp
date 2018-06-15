#pragma once


#include <boost/asio.hpp>

#include "message.inl"

#include "app_config.hpp"
#include "block_chain.hpp"
#include "node_account.hpp"
#include "node_message.inl"
#include "node_session.hpp"
#include "_vector.hpp"

// node status
enum node_state
{
	node_state_null = 0,

	node_state_sync,
	node_state_waiting,
	node_state_new_round,
	node_state_propose,
	node_state_vote,
	node_state_commit,

	node_state_end
};



// node server class
// 2 udp server + 1 tcp server: node p2p + node <-> game client + node <-> bridge
class node_server : public bryllite::net::peer_host, public bryllite::net::udp_server, public bryllite::net::tcp_server, public bryllite::timer_callback
{
	using udp = boost::asio::ip::udp;
	using tcp = boost::asio::ip::tcp;
	using udp_server = bryllite::net::udp_server;
	using tcp_server = bryllite::net::tcp_server;
	using tcp_session = bryllite::net::tcp_session;
	using block_header = bryllite::block_header;
	using block = bryllite::block;
	using transaction = bryllite::transaction;

public:
	// bridge server session
	class bridge_session : public tcp_session
	{
	public:
		bridge_session( boost::asio::io_service& ios ) : tcp_session(ios) {
		};
	};

	enum {
		timer_id_ping = 0,
		timer_id_travel,
		timer_id_state_machine,
		timer_id_for_pop,
		timer_id_for_propose_end,
	};

protected:
	app_config& _app_config;	// app config
	node_account _account;		// node account
	block_chain _block_chain;	// block list
	block _block_nominee;		// current working block

	bryllite::callback_timer _timer;


	node_state _node_state;
	time_t _node_state_begin_time;				// node state start time (ms)

	///////////////////////
	// node_state_sync
	///////////////////////
	time_t _sync_time;
	_vector<size_t> _sync_target_block_height;		// sync target block height

	//////////////////////
	// node_state_waiting
	//////////////////////
	_vector<size_t> _received_new_round;
	bool _new_round_message_sent;
	time_t _current_round_time;

	////////////////////////
	// node_state_new_round
	////////////////////////
	size_t _new_block_index;
	block _new_block;
	_vector<block_header> _user_block_header;		// user block header list

	/////////////////////////
	// node_state_propose
	/////////////////////////
	_vector<block_header> _propose_header;

	////////////////////////
	// node_state_vote
	////////////////////////
	_vector<block_header> _vote_header;

	////////////////////////
	// node_state_commit
	////////////////////////
	block_header _commit_block_header;
	block _commit_block;
	_vector<block> _commit;


public:
	node_server( app_config& app_config );

	// start/stop node_server
	bool start_server( void );
	void stop_server( void ) ;
	void main( void );

public:
	size_t current_block_height( void );

protected:

	void set_state( node_state state );
	node_state get_state( void );

protected:
	void onTimeOut( bryllite::timer_id tid, void* pContext );

protected:
	// tcp_session -> bridge_session allocator
	tcp_session* new_session( void );

	// peer_session -> node_session allocator
	node_session* new_peer_session( udp::endpoint ep );

protected:
	// node <-> node message handler
	void onPeerMessage( udp::endpoint peer, message* msg );
	void onPeerMessagePing( node_session* session, message_ping* msg );
	void onPeerMessagePong( node_session* session, message_pong* msg );

	// node <-> game client message handler
	void onUdpMessage( udp::endpoint endPoint, message* msg );
	void onUserMessagePing( const udp::endpoint& endPoint, message_ping* msg );
	void onUserMessagePong( const udp::endpoint& endPoint, message_pong* msg );
	void onUserMessageSignedTravel( const udp::endpoint& endPoint, message_signed_travel* msg );
	void onUserMessagePop( const udp::endpoint& endPoint, node_client_message_pop* msg );

	// node <-> bridge server message handler
	void onTcpMessage( tcp_session* session, message* msg );
	void onBridgeMessagePing( bridge_session* session, message_ping* msg );
	void onBridgeMessagePong( bridge_session* session, message_pong* msg );

protected:
	void on_state_begin( node_state state );
	void on_state_end( node_state state );
	void on_state_main( node_state state );

	void on_state_sync_begin( void );
	void on_state_waiting_begin( void );
	void on_state_new_round_begin( void );
	void on_state_propose_begin( void );
	void on_state_vote_begin( void );
	void on_state_commit_begin( void );

	void on_state_sync_main( void );
	void on_state_waiting_main( void );
	void on_state_new_round_main( void );
	void on_state_propose_main( void );
	void on_state_vote_main( void );
	void on_state_commit_main( void );

	void on_state_sync_end( void );
	void on_state_waiting_end( void );
	void on_state_new_round_end( void );
	void on_state_propose_end( void );
	void on_state_vote_end( void );
	void on_state_commit_end( void );

protected:
	void request_block_sync( void );
	bool request_new_round( size_t new_block_index );
	bool request_propose( block_header header );
	bool request_vote( block_header header );
	bool request_commit( block b );

	bool create_block_for( size_t block_index, block& b );
	bool get_block( size_t block_number, block& b );
	bool get_block_hash( size_t block_index, uint256& hash );

};


