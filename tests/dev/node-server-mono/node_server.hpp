#pragma once

#include "node_manager.hpp"
#include "node_message.inl"


class node_manager;
class node_server : public iface_bcp_server
{
	using block_header = bryllite::block_header;
	using block = bryllite::block;
	using transaction = bryllite::transaction;
	using account = bryllite::account;

protected:
	node_manager& _node_manager;

	// peer id
	int _peer_id;

	// node account
	account _node_account;	

	// block chain
	CBlockChain _block_chain;

	// bcp server
	bcp_server _bcp_server;

public:
	node_server( node_manager& manager, int peer_id );

	bool start_server( void );
	bool stop_server( void );
	int main( void );

	int peer_id( void )
	{
		return _peer_id;
	};
	int node_id( void )
	{
		return peer_id();
	};


	bool sendto( node_server* peer, message* msg );
	size_t sendall( message* msg );

protected:
	// create block
	bool create_block_for( size_t block_index, block& b );

public:
	int on_bcp_new_round_ready( void );
	int on_bcp_new_round_start_for( size_t block_round );
	int on_bcp_step_propose_start_for( void );
	int on_bcp_step_vote_start_for( void );
	int on_bcp_step_commit_start_for( block_header elected_block, bool my_votes );
	int on_bcp_new_block_commit_for( block commit_block );
	int on_bcp_consensus_fail( void );
	size_t all_peer_count( void );


	int onPeerMessage( node_server* peer, message* msg );

protected:
	int onPeerMessageBlockReq( node_server* peer, node_message_block_req* msg );
	int onPeerMessageBlockAck( node_server* peer, node_message_block_ack* msg );

	int onPeerMessageNewRound( node_server* peer, node_message_new_round* msg );
	int onPeerMessagePropose( node_server* peer, node_message_propose* msg );
	int onPeerMessageVote( node_server* peer, node_message_vote* msg );
	int onPeerMessageVerifyBlock( node_server* peer, node_message_verify_block* msg );
	int onPeerMessageCommit( node_server* peer, node_message_commit* msg );


};

