#include "stdafx.hpp"
#include "node_server.hpp"


node_server::node_server( node_manager& manager, int peer_id ) : _node_manager(manager), _bcp_server(*this), _peer_id(peer_id)
{
}

bool node_server::start_server( void )
{
	if( !_bcp_server.start() )
	{
		elog( "node_server(%d).bcp_server.start() failed", node_id() );
		return false;
	}

	// load node account or generate new
	std::string keyFile = bryllite::format( "node_%d.key", node_id() );
	if ( !_node_account.loadKeyFile( keyFile ) )
	{
		wlog( "node_server(%d): key file(%s) not found! generating new account", node_id(), keyFile.c_str() );

		// generate new key
		_node_account.generateNewAccount();
		_node_account.saveKeyFile( keyFile );
	}

	// node account information
	ulog( "[node_account]: key_file=%s, account_info=%s", keyFile.c_str(), _node_account.to_string().c_str() );

	return true;
}

bool node_server::stop_server( void )
{
	if ( !_bcp_server.stop() )
	{
		elog( "bcp_server.stop() failed" );
		return false;
	}

	return true;
}

int node_server::main( void )
{
	// bcp process
	_bcp_server.process();

	return 0;
}


bool node_server::sendto( node_server* peer, message* msg )
{
	return peer->onPeerMessage( this, msg );
}

size_t node_server::sendall( message* msg )
{
	return _node_manager.sendall( this, msg );
}


bool node_server::create_block_for( size_t block_index, block& b )
{
	if ( _block_chain.block_exists( block_index ) ) 
	{
		wlog( "node_server[%d]: create_block_for( block_index=%d ): block exists!", node_id(), block_index );
		return false;
	}

	// prev block hash
	uint256 prevBlockHash = _block_chain.get_block_hash( block_index - 1 );

	// new block
	bryllite::block new_block( block_index, prevBlockHash );

	// coinbase block reward
	uint64_t block_reward = 100;
	bryllite::transaction tx_coinbase( _node_account.address(), block_reward );
	tx_coinbase.sign( _node_account.privateKey(), _node_account.publicKey() );

	// append tx
	if ( !new_block.append_transaction( tx_coinbase ) )
	{
		wlog( "CNodeServer[%d]: append_transaction() failed. tx_coinbase=%s", node_id(), tx_coinbase.to_string().c_str() );
		return false;
	}

	// add transaction from transaction pool
//	add_transaction_from_pool( new_block );

	b = new_block;
	return true;
}


// peer message map for node message
#define		begin_peer_msg_map( _peer, _msg )		node_server* __peer__=_peer; message* __message__=_msg; switch( _msg->msgid() ) {
#define		entry_peer_msg_map( _msg, _proc )		case _msg##_id: {	_proc( __peer__, (_msg*)__message__ ); return 0;	}
#define		end_peer_msg_map()						default: break; }


int node_server::onPeerMessage( node_server* sender, message* msg )
{
	begin_peer_msg_map( sender, msg )
		entry_peer_msg_map( node_message_block_req, onPeerMessageBlockReq )
		entry_peer_msg_map( node_message_block_ack, onPeerMessageBlockAck )
		entry_peer_msg_map( node_message_new_round, onPeerMessageNewRound )
		entry_peer_msg_map( node_message_propose, onPeerMessagePropose )
		entry_peer_msg_map( node_message_vote, onPeerMessageVote )
		entry_peer_msg_map( node_message_commit, onPeerMessageCommit )
		entry_peer_msg_map( node_message_verify_block, onPeerMessageVerifyBlock )
	end_peer_msg_map()

	return 0;
}

int node_server::onPeerMessageBlockReq( node_server* sender, node_message_block_req* msg )
{
	size_t block_index = msg->_block_index;

	block b;
	if ( _block_chain.get_block( block_index, b ) )
	{
		node_message_block_ack ack(b);
		sendto( sender, &ack );
	}

	log( "CNodeServer[%d]: requesting block(%d) data!", peer_id(), block_index );

	return 0;
}

int node_server::onPeerMessageBlockAck( node_server* sender, node_message_block_ack* msg )
{
	block b = msg->get();

	log( "CNodeServer[%d]: Receiving block(%d) data!", peer_id(), b.block_index() );

	_block_chain.append_block( msg->get() );

	return 0;
}


int node_server::onPeerMessageNewRound( node_server* sender, node_message_new_round* msg )
{
//	log( "node_server[%d].onPeerMessageNewRound(peer=%d, msg->round_idx=%d", node_id(), sender->peer_id(), msg->_new_block_index );

	// append new round message
	_bcp_server.append_new_round_message_for( sender->peer_id(), msg->_new_block_index );

	return 0;
}

int node_server::onPeerMessagePropose( node_server* sender, node_message_propose* msg )
{
//	log( "node_server[%d].onPeerMessagePropose(peer=%d): header=%s", node_id(), sender->peer_id(), msg->_block_header.get_hash_string().substr(0, 4).c_str() );

	// append proposed block header
	_bcp_server.append_proposed_block_header_for( sender->peer_id(), msg->_block_header );

	return 0;
}

int node_server::onPeerMessageVote( node_server* sender, node_message_vote* msg )
{
//	log( "node_server[%d].onPeerMessageVote(peer=%d): header=%s", node_id(), sender->peer_id(), msg->_block_header.get_hash_string().substr(0, 4).c_str() );

	block_header vote_header = msg->_block_header;
	log( "node_server[%d]: node(%d) vote to %s", node_id(), sender->peer_id(), vote_header.get_hash_string().substr(0, 4).c_str() );

	// append block header vote
	_bcp_server.vote_block_header_for( sender->peer_id(), vote_header);

	return 0;
}

int node_server::onPeerMessageCommit( node_server* sender, node_message_commit* msg )
{
	block commit_block = msg->get();
	log( "node_server[%d]: node(%d) commit to %s", node_id(), sender->peer_id(), commit_block.get_hash_string().substr(0,4).c_str() );

	_bcp_server.add_verified_block( sender->peer_id(), commit_block );

	return 0;
}


int node_server::onPeerMessageVerifyBlock( node_server* sender, node_message_verify_block* msg )
{
	block block_to_verify = msg->get();

	dlog( "node_server[%d]: received verify_block=%s", node_id(), block_to_verify.get_hash_string().substr(0,4).c_str() );

	block_header elected_header;
	if ( !_bcp_server.get_elected_block_header( elected_header ) )
	{
		wlog( "node_server[%d].onPeerMessageVerifyBlock(): No elected block header! verify_block=%s", node_id(), block_to_verify.get_hash_string().substr(0,4).c_str() );
		return 0;
	}

	// is elected block?
	if ( elected_header != block_to_verify.header() )
	{
		wlog( "node_server[%d]: onPeerMessageVerifyBlock(): invalid verify block!", node_id() );
		return 0;
	}

	// verify block
	if ( !block_to_verify.verify() )
	{
		wlog( "node_server[%d]: onPeerMessageVerifyBlock(): block.verify() failed!", node_id() );
		return 0;
	}

	// add verify node-signing here
	// ...

	// send commit message
	node_message_commit commit( block_to_verify );
	sendall( &commit );

	return 0;
}


int node_server::on_bcp_new_round_ready( void )
{
	node_message_new_round new_round_msg( _block_chain.current_height() + 1 );
	sendall(&new_round_msg);

	return 0;
}

int node_server::on_bcp_new_round_start_for( size_t block_round )
{
	log( "node_server[%d]: New round for (%d) started", node_id(), block_round );

	block new_block;
	if ( !create_block_for( block_round, new_block ) )
	{
		wlog( "node_server[%d]: create_block_for(block_index=%d) failed", node_id(), block_round );
		return 0;
	}

	// sign block
	new_block.sign( _node_account.keyPair() );

	// set candidate block
	_bcp_server.candidate_block( new_block );

	// add node block header
	_bcp_server.append_user_block_header_for( _node_account.address(), new_block.header() );

	log( "node_server[%d]: candidate_block=%s", node_id(), new_block.get_hash_string().c_str() );

	return 0;
}

int node_server::on_bcp_step_propose_start_for( void )
{
	log( "node_server[%d]: <BCP> PROPOSE STEP STARTED", node_id() );

	std::string user_address;
	block_header user_header;
	if ( _bcp_server.select_user_block_header( user_address, user_header ) )
	{
		log( "node_server[%d]: selected block header: %s", node_id(), user_header.get_hash_string().c_str() );

		// change user header
		block& candidate_block = _bcp_server.candidate_block();
		candidate_block._header = user_header;

		// broadcast my propose
		node_message_propose propose( user_header );
		sendall( &propose );
	}

	return 0;
}

int node_server::on_bcp_step_vote_start_for( void )
{
	log( "node_server[%d]: <BCP> VOTE STEP STARTED", node_id() );

	block_header vote_header;
	if ( _bcp_server.select_proposed_block_header( vote_header ) )
	{
		// save my vote
		_bcp_server.set_my_vote(vote_header);

		// broadcast my vote
		node_message_vote vote( vote_header );
		sendall( &vote );
	}

	return 0;
}

int node_server::on_bcp_step_commit_start_for( block_header elected_header, bool my_votes )
{
	log( "node_server[%d]: COMMIT STEP STARTED: my_votes=%d", node_id(), my_votes );

	if ( my_votes )
	{
		block block_to_verify = _bcp_server.candidate_block();
		node_message_verify_block verify( block_to_verify );
		sendall( &verify );
	}

	return 0;
}

int node_server::on_bcp_new_block_commit_for( block commit_block )
{
	log( "node_server[%d]: <BCP> NEW BLOCK!!", node_id() );

	_block_chain.append_block( commit_block );

	std::string node_address = _node_account.address();
	ulog( "balanceOf(%s): %lld BRC", node_address.c_str(), _block_chain.balanceOf( node_address ) );

	_bcp_server.clear_intermediate_data();

	return 0;
}

int node_server::on_bcp_consensus_fail( void )
{
	wlog( "node_server[%d]: <BCP> CONSENSUS FAILED", node_id() );

	// clear intermediate data
	_bcp_server.clear_intermediate_data();

	return 0;
}

size_t node_server::all_peer_count( void )
{
	return _node_manager.all_peer_count();
}


