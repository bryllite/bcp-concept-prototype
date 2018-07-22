#include "node.hpp"
#include "node_manager.hpp"



const char* get_node_state_string( node_state state )
{
	switch( state )
	{
	case node_state_null: return "null";
	case node_state_sync: return "sync";
	case node_state_waiting: return "waiting";
	case node_state_new_round: return "new_round(pop)";
	case node_state_propose: return "propose";
	case node_state_vote: return "vote";
	case node_state_commit: return "commit";
	default: break ;
	}

	return "unknown";
}

time_t node::get_current_round_time(time_t& round)
{
	time_t current_sec = _ntp_timer.timestamp();
	round = current_sec / bcp_block_time;
	return current_sec % bcp_block_time;
};

time_t node::get_current_round_time(void)
{
	time_t round;
	return get_current_round_time(round);
};



node::node(node_manager& node_manager) 
	: _node_manager(node_manager), _idx(-1), _node_state(node_state_null), _node_state_begin_time(0)
{
	_node_manager.append_node(this);
}

bool node::start( int idx )
{
	_idx = idx;

	// generate key pair ( account, public key, private key )
	_key_pair.generate_new();
	log( "node[%d]: address=%s", idx, _key_pair.address().c_str() );

	// set sync mode
	set_state( node_state_sync );

	// ntp timer synchronize
	if (!_ntp_timer.sync())
	{
		elog("node[%d]: failed to sync ntp", idx);
		return false;
	}

	log( "node[%d]: started", idx );

	return true;
}

void node::sendto( node* receiver, message* msg )
{
	_node_manager.sendto( receiver, msg, this );
}

void node::sendall( message* msg )
{
	_node_manager.sendall( msg, this );
}

void node::set_state( node_state s )
{
	if ( _node_state == s ) return ;

	on_state_end( _node_state );

	_node_state = s;
	_node_state_begin_time = bryllite::timestamp();

	log( "node[%d]: STATE='%s', blocks.size()=%d", idx(), get_node_state_string( _node_state ), _block_chain.size() );

	on_state_begin( _node_state );
}

bool node::append_block( block b )
{
	size_t block_index = b.block_index();
	if ( _block_chain.append_block(block_index, b, true) )
	{
		dlog( "node[%d]: new block(%d) appended. block.hash=%s, block.prevHash=%s", idx(), block_index, b.get_header_hash().GetHex().c_str(), b.get_prev_hash().GetHex().c_str() );
		ulog( "node[%d]: balanceOf(%s)=%lld", idx(), _key_pair.address().c_str(), balanceOf( _key_pair.address() ) );
		return true;
	}

	return false;
}


int node::onMain( void )
{
	// call state main
	on_state_main( _node_state );

	return 0;
}

void node::request_block_sync( void )
{
	_sync_target_block_height.clear();

	// request current block height
	node_message_block_height_req req( _block_chain.size() );
	sendall( &req );
}

bool node::request_new_round( size_t new_block_index )
{
	time_t round_time;
	time_t step_time = get_current_round_time( round_time );

	if ( _current_round_time < round_time ) 
	{
		_current_round_time = round_time;
		_new_round_message_sent = false;
	}

	if ( !_new_round_message_sent && step_time < bcp_timeout_pop )
	{
		_new_round_message_sent = true;
		node_message_new_round req( new_block_index );
		sendall( &req );

		return true;
	}

	return false;
}

bool node::request_propose( block_header header )
{
	node_message_propose propose( header );
	sendall( &propose );

	return true;
}

bool node::request_vote( block_header header )
{
	node_message_vote vote( header );
	sendall( &vote );

	return true;
}

bool node::request_commit( block b )
{
	node_message_commit commit(b);
	sendall( &commit );

	return true;
}


void node::on_state_sync_begin( void )
{
	_sync_time = bryllite::timestamp();
}


void node::on_state_sync_main( void )
{
	// do not sync yet.
	set_state( node_state_waiting );

	if( bryllite::timestamp() - _sync_time > 1000 )
	{
		_sync_time = bryllite::timestamp();

		request_block_sync();
		return;
	}

	// check whether block sync completed
	size_t block_height;
	if( _sync_target_block_height.get_value_if_count_exceeds( block_height, _node_manager.get_peer_count() / 2 ) )
	{
		// sync completed
		if( _block_chain.size() >= block_height )
		{
			//				log( "node[%d]: sync completed", idx() );
			set_state( node_state_waiting );
		}
	}
}

void node::on_state_sync_end( void )
{
}


void node::on_state_waiting_begin( void )
{
	_received_new_round.clear();
	_user_block_header.clear();
	_propose_header.clear();
	_vote_header.clear();
	_commit.clear();
}

void node::on_state_waiting_main( void )
{
	size_t new_block_index = _block_chain.size() + 1;

	// send new round message
	request_new_round( new_block_index );

	// check whether new round begin
	size_t block_index;
	if( _received_new_round.get_value_if_count_exceeds( block_index, _node_manager.get_peer_count() / 2 ) )
	{
		if( new_block_index == block_index )
		{
			_new_block_index = block_index;
			set_state( node_state_new_round );
		}
		else if( new_block_index < block_index )
		{
			set_state( node_state_sync );
		}
	}
}

void node::on_state_waiting_end( void )
{
}

void node::on_state_new_round_begin( void )
{
	if( !create_block_for( _new_block_index, _new_block ) )
	{
		wlog( "node[%d]: create_block_for( block_index: %d ) failed", idx(), _new_block_index );
		_node_manager.stop();
		return;
	}

	// sign
	_new_block.sign( _key_pair.get_private_key(), _key_pair.get_public_key() );

	// append user block list
	_user_block_header.add( _new_block._header );

	dlog( "node[%d]: block(%d) nominated. hash=%s", idx(), _new_block.block_index(), _new_block.get_header_hash().GetHex().c_str() );
}


void node::on_state_new_round_main( void )
{
	// wait for proof of participation completed
	if( get_current_round_time() > bcp_timeout_pop ) 
	{
		set_state( node_state_propose );
	}
}

void node::on_state_new_round_end( void )
{
}

void node::on_state_propose_begin( void )
{
	block_header h;
	if( !_user_block_header.low_value( h ) )
	{
		wlog( "node[%d]: _user_block_header.low_value() failed", idx() );
		return;
	}

	// send propose to all
	request_propose( h );
}

void node::on_state_propose_main( void )
{
	// wait for propose step completed
	if( get_current_round_time() > bcp_timeout_propose ) 
	{
		set_state( node_state_vote );
	}
}

void node::on_state_propose_end( void )
{
}


void node::on_state_vote_begin( void )
{
	block_header h;

	if( !_propose_header.low_value( h ) )
	{
		wlog( "node[%d]: _propose_header.low_value() failed", idx() );
		return;
	}

	// vote to header 
	request_vote( h );
}

void node::on_state_vote_main( void )
{
	// check timeout
	if( get_current_round_time() > bcp_timeout_vote ) 
	{
		wlog( "node[%d]: consensus failed on vote step: timeout", idx() );
		set_state( node_state_waiting );
		return;
	}

	// find out +1/2 voted header
	block_header h;
	if( _vote_header.get_value_if_count_exceeds( h, _node_manager.get_peer_count() / 2 ) ) 
	{
		_commit_block_header = h;

		dlog( "node[%d]: +1/2 voted block_header: %s", idx(), _commit_block_header.get_header_hash().GetHex().c_str() );
		set_state( node_state_commit );
	}
}

void node::on_state_vote_end( void )
{
}


void node::on_state_commit_begin( void )
{
	// request for commit block data
	node_message_commit_block_req req( _commit_block_header );
	sendall( &req );
}

void node::on_state_commit_main( void )
{
	// check timeout
	if( get_current_round_time() <= 1 ) {
		wlog( "node[%d]: consensus failed on commit step: timeout", idx() );
		set_state( node_state_waiting );
		return;
	}

	// find out +1/2 commited block
	block b;
	if ( _commit.get_value_if_count_exceeds( b, _node_manager.get_peer_count()/2 ) )
	{
		dlog( "node[%d]: +1/2 commited block_header: %s", idx(), b.get_header_hash().GetHex().c_str() );

		// append block
		if ( !append_block(b) ) {
			wlog( "node[%d]: append_block() failed", idx() );
		}

		set_state( node_state_waiting );
	}
}

void node::on_state_commit_end( void )
{
}


void node::on_state_begin( node_state state )
{
	switch( state )
	{
		case node_state_sync: return on_state_sync_begin(); return;
		case node_state_waiting: return on_state_waiting_begin(); return;
		case node_state_new_round: return on_state_new_round_begin(); return;
		case node_state_propose: return on_state_propose_begin(); return;
		case node_state_vote: return on_state_vote_begin(); return;
		case node_state_commit: return on_state_commit_begin(); return;
		default: break;
	}

	wlog( "node[%d]: on_state_begin(): unknown state(%d)", idx(), state );
}

void node::on_state_end( node_state state )
{
	switch( state )
	{
		case node_state_null: return;
		case node_state_sync: on_state_sync_end(); return;
		case node_state_waiting: on_state_waiting_end(); return;
		case node_state_new_round: on_state_new_round_end(); return;
		case node_state_propose: on_state_propose_end(); return;
		case node_state_vote: on_state_vote_end(); return;
		case node_state_commit: on_state_commit_end(); return;
		default: break;
	}

	wlog( "node[%d]: on_state_end(): unknown state(%d)", idx(), state );
}

void node::on_state_main( node_state state )
{
	switch( state ) 
	{
		case node_state_sync: on_state_sync_main(); return ;
		case node_state_waiting: on_state_waiting_main(); return ;
		case node_state_new_round: on_state_new_round_main(); return ;
		case node_state_propose: on_state_propose_main(); return ;
		case node_state_vote: on_state_vote_main(); return;
		case node_state_commit: on_state_commit_main(); return;
		default: break ;
	}

	wlog( "node[%d]: on_state_main(): unknown state(%d)", idx(), state );
}



// node message map for node message
#define		begin_node_msg_map( _sender, _msg )		node* __sender__=_sender; message* __message__=_msg; switch( _msg->msgid() ) {
#define		entry_node_msg_map( _msg, _proc )		case _msg##_id: {	return _proc( __sender__, (_msg*)__message__ ); }
#define		end_node_msg_map()						default: break; }

// node message handler
int node::onMessage( node* sender, message* msg )
{
	begin_node_msg_map( sender, msg )
		entry_node_msg_map( node_message_block_height_req, onMessageBlockHeightReq )
		entry_node_msg_map( node_message_block_height_ack, onMessageBlockHeightAck )
		entry_node_msg_map( node_message_block_req, onMessageBlockReq )
		entry_node_msg_map( node_message_block_ack, onMessageBlockAck )
		entry_node_msg_map( node_message_new_round, onMessageNewRound )
		entry_node_msg_map( node_message_propose, onMessagePropose )
		entry_node_msg_map( node_message_vote, onMessageVote )
		entry_node_msg_map( node_message_commit, onMessageCommit )
		entry_node_msg_map( node_message_commit_block_req, onMessageCommitBlockReq )
		entry_node_msg_map( node_message_commit_block_ack, onMessageCommitBlockAck )
	end_node_msg_map()

	return 0;
}


int node::onMessageBlockHeightReq( node* sender, node_message_block_height_req* msg )
{
	// ack current block height
	node_message_block_height_ack ack( _block_chain.size() );
	sendto( sender, &ack );

	return 0;
}

int node::onMessageBlockHeightAck( node* sender, node_message_block_height_ack* msg )
{
//	log( "node[%d]: onMessageBlockHeightAck(): sender=%d, block_height=%d", idx(), sender->idx(), msg->_block_height );

	_sync_target_block_height.add( msg->_block_height );

	return 0;
}

int node::onMessageBlockReq( node* sender, node_message_block_req* msg )
{

	return 0;
}

int node::onMessageBlockAck( node* sender, node_message_block_ack* msg )
{

	return 0;
}

int node::onMessageNewRound( node* sender, node_message_new_round* msg )
{
//	log( "node[%d]: onMessageNewRound(): sender=%d, new_block_index=%d", idx(), sender->idx(), msg->_new_block_index );

	_received_new_round.add( msg->_new_block_index );

	return 0;
}

int node::onMessagePropose( node* sender, node_message_propose* msg )
{
//	log( "node[%d]: onMessagePropose(): sender=%d, block_header=%s", idx(), sender->idx(), msg->_block_header.dump().c_str() );

	_propose_header.add( msg->_block_header );

	return 0;
}

int node::onMessageVote( node* sender, node_message_vote* msg )
{
//	log( "node[%d]: onMessageVote(): sender=%d, block_header=%s", idx(), sender->idx(), msg->_block_header.dump().c_str() );

	_vote_header.add( msg->_block_header );

	return 0;
}

int node::onMessageCommit( node* sender, node_message_commit* msg )
{
	block b = msg->get();
//	dlog( "node[%d]: onMessageCommit(): sender=%d, block.idx=%d", idx(), sender->idx(), b.blockNumber() );

	_commit.add( b );

	return 0;
}

int node::onMessageCommitBlockReq( node* sender, node_message_commit_block_req* msg )
{
//	dlog( "node[%d]: onMessageCommitBlockReq(): sender=%d, block_idx=%d", idx(), sender->idx(), msg->_block_header.blockNumber() );

	if ( msg->_block_header == _new_block._header ) {
		node_message_commit_block_ack ack( _new_block );
		sendto( sender, &ack );
	}

	return 0;
}

int node::onMessageCommitBlockAck( node* sender, node_message_commit_block_ack* msg )
{
	_commit_block = msg->get();
//	dlog( "node[%d]: onMessageCommitBlockAck(): sender=%d, block_idx=%d", idx(), sender->idx(), _commit_block.blockNumber() );

	// validation
	if ( _commit_block.valid() && _commit_block._header == _commit_block_header && _commit_block.merkle_root_hash() == _commit_block._header._header._hashMerkleRoot ) 
	{
		request_commit( _commit_block );
	}

	return 0;
}



bool node::get_block( size_t block_index, block& b )
{
	return _block_chain.get_block( block_index, b );
};

bool node::get_block_hash( size_t block_index, uint256& hash )
{
	block b;
	if ( get_block( block_index, b ) )
	{
		hash = b.get_header_hash();
		return true;
	}

	return false;
};


bool node::create_block_for( size_t block_index, block& b )
{
	uint256 hashPrevBlock;
	if ( block_index > 1 && !get_block_hash( block_index - 1, hashPrevBlock ) ) {
		wlog( "node[%d]: create_block_for(%d): hashPrevBlock not found", idx(), block_index );
		return false;
	}

	bryllite::block new_block( block_index, hashPrevBlock );

	// block reward 
	size_t block_reward = 100;
	transaction tx_reward( _key_pair.address(), _key_pair.address(), block_reward );
	tx_reward.sign( _key_pair.get_private_key(), _key_pair.get_public_key() );

	// append block reward tx
	if ( !new_block.append_transaction( tx_reward ) ) {
		wlog( "node[%d]: append_transaction() failed. tx_reward=%s", idx(), tx_reward.to_string().c_str() );
		return false;
	}
/*
	// append memory pooled tx here...
	// random transaction
	int tx_cnt = rand() % 10;
	for( int i = 0; i<tx_cnt; i++ )
	{
		bryllite::key_pair from, to;
		from.generate_new();
		to.generate_new();

		transaction tx( from.address(), to.address(), rand() % 1000 );
		tx.sign( from.get_private_key(), from.get_public_key() );
		new_block.append_transaction( tx );
	}
*/
	b = new_block;
	return true;
};


// get balance of account
uint64_t node::balanceOf( std::string address )
{
	return _block_chain.balanceOf( address );
};
