#pragma once

enum bcp_step
{
	bcp_step_null = 0,
	bcp_step_waiting,
	bcp_step_proof_of_participation,
	bcp_step_propose,
	bcp_step_vote,
	bcp_step_commit,
};

enum bcp_timeout
{
#ifdef _DEBUG
	bcp_timeout_proof_of_participation = 4,
	bcp_timeout_propose = 6,
	bcp_timeout_vote = 8,
	bcp_timeout_commit = 10,
#else
	bcp_timeout_proof_of_participation = 4,
	bcp_timeout_propose = 6,
	bcp_timeout_vote = 8,
	bcp_timeout_commit = 10,
#endif

	bcp_block_time = bcp_timeout_commit
};

// bcp server callback interface
class iface_bcp_server
{
public:
	virtual int on_bcp_new_round_ready( void ) = 0;
	virtual int on_bcp_new_round_start_for( size_t block_round ) = 0;
	virtual int on_bcp_step_propose_start_for( void ) = 0;
	virtual int on_bcp_step_vote_start_for( void ) = 0;
	virtual int on_bcp_step_commit_start_for( void ) = 0;
	virtual int on_bcp_new_block_commit_for( void ) = 0;
	virtual int on_bcp_consensus_fail( void ) = 0;

public:
	virtual size_t all_peer_count( void ) = 0;
};

// bcp server class
class bcp_server : public bryllite::lockable
{
	using ntp_timer = bryllite::ntp_timer;

protected:
	// bcp server callback interface
	iface_bcp_server& _bcp_server_callback;

	// ntp timer
	ntp_timer _ntp_timer;
	time_t _time_context;

	// current bcp step
	bcp_step _current_step;

	// new round data
	std::map< int, size_t > _new_round_messages;
	std::map< size_t, size_t > _new_round_votes;


public:
	bcp_server(iface_bcp_server& bcp_server_callback );

	// start/stop bcp server
	bool start( void );
	bool stop( void );

	// process bcp logic
	int process( void );

public:

	// time line
	time_t timeline( time_t& time_context );
	time_t timeline( void );

	// bcp step
	bcp_step current_step( void );
	void current_step( bcp_step step );

	// clear intermediate data
	void clear_intermediate_data( void );

protected:

	// receiving new round message
	bool append_new_round_message_for( int peer_id, size_t round_idx );

	// is new round start for?
	bool new_round_start_for( size_t& round_idx );
};

