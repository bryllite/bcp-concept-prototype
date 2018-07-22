#include "stdafx.hpp"
#include "bcp_server.hpp"

bcp_server::bcp_server(iface_bcp_server& bcp_server_callback) : _bcp_server_callback(bcp_server_callback), _time_context(0), _current_step(bcp_step_null), _current_round(invalid_block_index)
{
}

int bcp_server::node_id( void )
{
	return _bcp_server_callback.node_id();
}


// clear intermediate data
void bcp_server::clear_intermediate_data( void )
{
	smartlock( this );

	_new_round_messages.clear();
	_new_round_votes.clear();
	_current_round = invalid_block_index;

	_user_block_headers.clear();

	_proposed_block_headers.clear();

	_vote_box.clear();
	_vote_headers.clear();
	_vote_counts.clear();

	_verified_blocks.clear();
}

bool bcp_server::start( void )
{
	// sync ntp_timer
	if ( !_ntp_timer.sync() )
	{
		elog( "bcp_server::start(): _ntp_timer.sync() failed" );
		return false;
	}

	// set bcp step waiting...
	current_step( bcp_step_waiting );

	return true;
}

bool bcp_server::stop( void )
{
	return true;
}

// timestamp on ntp
time_t bcp_server::timeline( time_t& time_context )
{
	time_t time_stamp = _ntp_timer.timestamp();
	time_context = time_stamp / bcp_block_time ;
	return time_stamp % bcp_block_time;
}

time_t bcp_server::timeline( void )
{
	time_t time_context = 0;
	return timeline( time_context );
}

bcp_step bcp_server::current_step( void )
{
	return _current_step;
}

void bcp_server::current_step( bcp_step step )
{
	_current_step = step;
}

int bcp_server::process( void )
{
	if ( current_step() == bcp_step_null )
		return 0;

	time_t time_context;
	time_t time_line = timeline( time_context );

	if ( current_step() == bcp_step_waiting )
	{
		if ( time_line < bcp_timeout_proof_of_participation )
		{
			// request new round message
			if ( time_context > _time_context )
			{
				_time_context = time_context;
				return _bcp_server_callback.on_bcp_new_round_ready();
			}

			// is new round started?
			size_t new_round_idx = invalid_block_index;
			if ( new_round_start_for( new_round_idx ) )
			{
				// node_server callback
				_bcp_server_callback.on_bcp_new_round_start_for( new_round_idx );
				current_step( bcp_step_proof_of_participation );
				return 0;
			}
		}
	}
	else if ( current_step() == bcp_step_proof_of_participation )
	{
		if ( time_line > bcp_timeout_proof_of_participation )
		{
			_bcp_server_callback.on_bcp_step_propose_start_for();
			current_step( bcp_step_propose );
			return 0;
		}
	}
	else if ( current_step() == bcp_step_propose )
	{
		if ( time_line > bcp_timeout_propose )
		{
			_bcp_server_callback.on_bcp_step_vote_start_for();
			current_step( bcp_step_vote );
			return 0;
		}
	}
	else if ( current_step() == bcp_step_vote )
	{
		if ( time_line > bcp_timeout_vote )
		{
			_bcp_server_callback.on_bcp_consensus_fail();
			current_step( bcp_step_waiting );
			return 0;
		}

		// find selected header
		block_header elected_header;
		if ( get_elected_block_header( elected_header ) )
		{
			_bcp_server_callback.on_bcp_step_commit_start_for(elected_header, elected_header == candidate_block()._header);
			current_step( bcp_step_commit );
			return 0;
		}
	}
	else if ( current_step() == bcp_step_commit )
	{
		if ( time_line < bcp_timeout_proof_of_participation )
		{
			_bcp_server_callback.on_bcp_consensus_fail();
			current_step( bcp_step_waiting );
			return 0;
		}

		block commit_block;
		if ( find_commit_block( commit_block ) )
		{
			_bcp_server_callback.on_bcp_new_block_commit_for( commit_block );
			current_step( bcp_step_waiting );
			return 0;
		}
	}

	return 0;
}

// append new round message for
bool bcp_server::append_new_round_message_for( int peer_id, size_t round_idx )
{
	smartlock(this);

	// new round message exists?
	if ( _new_round_messages.count( peer_id ) > 0 ) return false;

	_new_round_messages.insert( std::make_pair( peer_id, round_idx ) );
	_new_round_votes.count( round_idx ) == 0 ? _new_round_votes[round_idx] = 1 : _new_round_votes[round_idx]++ ;

	return true;
}

// is new round started for?
bool bcp_server::new_round_start_for( size_t& round_idx )
{
	size_t qualify_votes = _bcp_server_callback.all_peer_count();

	for( auto& pair : _new_round_votes )
	{
		size_t block_round = pair.first;
		size_t votes = pair.second;

		if ( votes >= qualify_votes && block_round > invalid_block_index )
		{
			round_idx = block_round;
			return true;
		}
	}

	return false;
}


void bcp_server::candidate_block( block b )
{
	_candidate_block = b;
}

bryllite::block& bcp_server::candidate_block( void )
{
	return _candidate_block;
}

void bcp_server::set_my_vote( block_header vote_header )
{
	_my_vote_header = vote_header;
}

bryllite::block_header bcp_server::get_my_vote( void )
{
	return _my_vote_header;
}

// append user block header ( proof of participation )
bool bcp_server::append_user_block_header_for( std::string address, block_header header )
{
	if ( _user_block_headers.count( address ) > 0 ) return false;

	_user_block_headers.insert( std::make_pair( address, header ) );
	return true;
}

// select lowest hash user block header
bool bcp_server::select_user_block_header( std::string& user_address, block_header& user_header )
{
	bool selected = false;
	if ( _user_block_headers.size() == 0 ) return false;

	for( auto& pair : _user_block_headers )
	{
		std::string addr = pair.first;
		block_header header = pair.second;

		if ( !selected )
		{
			selected = true;
			user_header = header;
			user_address = addr;
			continue ;
		}

//		if ( header < user_header )
		if ( header.get_hash_string().compare( user_header.get_hash_string() ) < 0 )
		{
			user_header = header;
			user_address = addr;
		}
	}

	return true;
}

// add proposed header
bool bcp_server::append_proposed_block_header_for( int peer_id, block_header proposed_header )
{
	if ( _proposed_block_headers.count( peer_id ) > 0 ) return false;

	_proposed_block_headers.insert( std::make_pair( peer_id, proposed_header ) );
	return true;
}

// select proposed header
bool bcp_server::select_proposed_block_header( block_header& selected_header )
{
	bool selected = false;
	if ( _proposed_block_headers.size() == 0 ) return false;

	for( auto& pair : _proposed_block_headers )
	{
		int peer_id = pair.first;
		block_header header = pair.second;

		if ( !selected )
		{
			selected = true;
			selected_header = header;
			continue;
		}

//		if ( header < selected_header )
		if ( header.get_hash_string().compare( selected_header.get_hash_string() ) < 0 )
			selected_header = header;
	}

	return true;
}

// append vote list
bool bcp_server::vote_block_header_for( int peer_id, block_header vote_header )
{
	if ( _vote_box.count( peer_id ) > 0 ) return false;

	_vote_box.insert( std::make_pair(peer_id, vote_header) );

	uint256 hash = vote_header.get_header_hash();

	// save header
	if ( _vote_headers.count(hash) == 0 ) 
		_vote_headers.insert( std::make_pair( hash, vote_header ) );

	// count votes
	_vote_counts.count(hash) == 0 ? _vote_counts[hash] = 1 : _vote_counts[hash]++ ;

	return true;
}

// get elected block header ( +1/2 votes )
bool bcp_server::get_elected_block_header( block_header& elected_header )
{
	size_t qualify_votes = ( _bcp_server_callback.all_peer_count() / 2 ) + 1 ;

	for( auto& pair : _vote_counts )
	{
		uint256 hash = pair.first;
		size_t votes = pair.second;

		if ( votes >= qualify_votes )
		{
			elected_header = _vote_headers[hash];
			return true;
		}
	}

	return false;
}


bool bcp_server::add_verified_block( int peer_id, block verified_block )
{
	if ( _verified_blocks.count( peer_id ) > 0 ) return false;

	block_header elected_header;
	if ( get_elected_block_header( elected_header ) && verified_block._header == elected_header )
	{
		_verified_blocks.insert( std::make_pair( peer_id, verified_block ) );
		_commit_block = verified_block;
		return true;
	}

	wlog( "bcp_server[%d]: add_verified_block(peer_id=%d) failed", node_id(), peer_id );
	return false;
}

bool bcp_server::find_commit_block( block& commit_block )
{
	size_t qualify_votes = ( _bcp_server_callback.all_peer_count()/2 ) + 1 ;

	if ( _verified_blocks.size() >= qualify_votes )
	{
		commit_block = _commit_block;
		return true;
	}

	return false;
}
