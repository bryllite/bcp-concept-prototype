#pragma once

#include <map>

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

#include "node_message.inl"

#include "_vector.hpp"
#include "block_chain.hpp"


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

// bcp timeout
enum bcp_timeout 
{
	bcp_timeout_pop = 5,
	bcp_timeout_propose = 10,
	bcp_timeout_vote = 20,
	bcp_timeout_commit = 30,

	bcp_block_time = bcp_timeout_commit
};


// node_manager class
class node_manager;

// node class
class node
{
	using block = bryllite::block;
	using block_header = bryllite::block_header;
	using transaction = bryllite::transaction;
	using address = bryllite::address;
	using ntp_timer = bryllite::ntp_timer;

protected:
	node_manager& _node_manager ;
	int _idx;

	bryllite::key_pair _key_pair;				// node key pair
	block_chain _block_chain;					// block chain

	ntp_timer _ntp_timer;

	node_state _node_state;						// node current state
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
	std::map< size_t, size_t > _map_new_round_message;	// new round messages list

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
	// ctor
	node( node_manager& _node_manager );

	// start node
	bool start( int idx );

	// node unique idx
	int idx( void ) { return _idx; };

	// sendto/all
	void sendto( node* receiver, message* msg );
	void sendall( message* msg );

	// change current state
	void set_state( node_state );
	node_state get_state( void ) { return _node_state; };

	time_t get_current_round_time(time_t& round);
	time_t get_current_round_time(void);

protected:
	void request_block_sync( void );
	bool request_new_round( size_t new_block_index );
	bool request_propose( block_header header );
	bool request_vote( block_header header );
	bool request_commit( block b );


protected:
	bool create_block_for( size_t block_index, block& b );
	bool get_block( size_t block_number, block& b );
	bool get_block_hash( size_t block_index, uint256& hash );

	// get balance of account
	uint64_t balanceOf( std::string address );

protected:
	void on_state_begin( node_state state );
	void on_state_end( node_state state );
	void on_state_main( node_state state );

	// state begin callback
	void on_state_sync_begin( void );
	void on_state_waiting_begin( void );
	void on_state_new_round_begin( void );
	void on_state_propose_begin( void );
	void on_state_vote_begin( void );
	void on_state_commit_begin( void );

	// state main callback
	void on_state_sync_main( void );
	void on_state_waiting_main( void );
	void on_state_new_round_main( void );
	void on_state_propose_main( void );
	void on_state_vote_main( void );
	void on_state_commit_main( void );

	// state end callback
	void on_state_sync_end( void );
	void on_state_waiting_end( void );
	void on_state_new_round_end( void );
	void on_state_propose_end( void );
	void on_state_vote_end( void );
	void on_state_commit_end( void );


public:
	int onMain( void );

public:
	int onMessage( node* sender, message* msg );
protected:
	int onMessageBlockHeightReq( node* sender, node_message_block_height_req* msg );
	int onMessageBlockHeightAck( node* sender, node_message_block_height_ack* msg );
	int onMessageBlockReq( node* sender, node_message_block_req* msg );
	int onMessageBlockAck( node* sender, node_message_block_ack* msg );
	int onMessageNewRound( node* sender, node_message_new_round* msg );
	int onMessagePropose( node* sender, node_message_propose* msg );
	int onMessageVote( node* sender, node_message_vote* msg );
	int onMessageCommit( node* sender, node_message_commit* msg );
	int onMessageCommitBlockReq( node* sender, node_message_commit_block_req* msg );
	int onMessageCommitBlockAck( node* sender, node_message_commit_block_ack* msg );

protected:
	bool append_block( block b );

};
