#include <bryllite-core.hpp>

#include "bcp_server.hpp"

bcp_server::bcp_server(iface_bcp_server& bcp_server_callback) : _bcp_server_callback(bcp_server_callback), _time_context(0), _current_step(bcp_step_null)
{
}

// clear intermediate data
void bcp_server::clear_intermediate_data( void )
{
	smartlock( this );

	_new_round_messages.clear();
	_new_round_votes.clear();
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
	return false;
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
				current_step( bcp_step_proof_of_participation );

				// node_server callback
				return _bcp_server_callback.on_bcp_new_round_start_for( new_round_idx );
			}
		}
	}
	else if ( current_step() == bcp_step_proof_of_participation )
	{
		if ( time_line > bcp_timeout_propose )
		{
			current_step( bcp_step_waiting );
			return 0;
		}

		if ( time_line > bcp_timeout_proof_of_participation )
		{
			current_step( bcp_step_propose );
			return _bcp_server_callback.on_bcp_step_propose_start_for();
		}
	}
	else if ( current_step() == bcp_step_propose )
	{
		if ( time_line > bcp_timeout_propose )
		{
			current_step( bcp_step_vote );
			return _bcp_server_callback.on_bcp_step_vote_start_for();
		}
	}
	else if ( current_step() == bcp_step_vote )
	{
	}
	else if ( current_step() == bcp_step_commit )
	{
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
