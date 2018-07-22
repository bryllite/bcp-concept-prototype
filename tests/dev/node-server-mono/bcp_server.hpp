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

#define USE_SHORT_BLOCK_TIME

enum bcp_timeout
{
#ifdef USE_SHORT_BLOCK_TIME
	bcp_timeout_proof_of_participation = 5,
	bcp_timeout_propose = 10,
	bcp_timeout_vote = 15,
	bcp_timeout_commit = 20,
#else
	bcp_timeout_proof_of_participation = 5,
	bcp_timeout_propose = 10,
	bcp_timeout_vote = 20,
	bcp_timeout_commit = 30,
#endif

	bcp_block_time = bcp_timeout_commit
};

// bcp server callback interface
class iface_bcp_server
{
	using block_header = bryllite::block_header;
	using block = bryllite::block;
public:
	virtual int on_bcp_new_round_ready( void ) = 0;
	virtual int on_bcp_new_round_start_for( size_t block_round ) = 0;
	virtual int on_bcp_step_propose_start_for( void ) = 0;
	virtual int on_bcp_step_vote_start_for( void ) = 0;
	virtual int on_bcp_step_commit_start_for( block_header elected_header, bool my_votes ) = 0;
	virtual int on_bcp_new_block_commit_for( block commit_block ) = 0;
	virtual int on_bcp_consensus_fail( void ) = 0;

public:
	virtual size_t all_peer_count( void ) = 0;
	virtual int node_id( void ) = 0;
};

// bcp server class
class bcp_server : public bryllite::lockable
{
	using ntp_timer = bryllite::ntp_timer;
	using block_header = bryllite::block_header;
	using block = bryllite::block;

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
	size_t _current_round;

	// candidate block
	block _candidate_block;

	// proof of participation data
	std::map< std::string, block_header > _user_block_headers; // <address,block_header>

	// proposed block headers
	std::map< int, block_header > _proposed_block_headers;	// <peer_id, block_header>

	// my voted header
	block_header _my_vote_header;

	// vote box
	std::map< int, block_header > _vote_box;				// peer_id, block_header
	std::map< uint256, block_header > _vote_headers;		// hash, block_header
	std::map< uint256, size_t > _vote_counts;				// hash, total vote count

	// verified blocks
	std::map< int, block > _verified_blocks;				// peer_id, block
	block _commit_block;

public:
	bcp_server(iface_bcp_server& bcp_server_callback );

	// start/stop bcp server
	bool start( void );
	bool stop( void );

	// process bcp logic
	int process( void );

	int node_id( void );

public:

	// time line
	time_t timeline( time_t& time_context );
	time_t timeline( void );

	// bcp step
	bcp_step current_step( void );
	void current_step( bcp_step step );

	// clear intermediate data
	void clear_intermediate_data( void );

	// candidate block
	void candidate_block( block b );
	block& candidate_block( void );

	void set_my_vote( block_header vote_header );
	block_header get_my_vote( void );

public:
	// receiving new round message
	bool append_new_round_message_for( int peer_id, size_t round_idx );

	// is new round start for?
	bool new_round_start_for( size_t& round_idx );

	// append user block header ( proof of participation )
	bool append_user_block_header_for( std::string address, block_header header );
	
	// select lowest block hash header
	bool select_user_block_header( std::string& user_address, block_header& user_header );

	// append proposed block header
	bool append_proposed_block_header_for( int peer_id, block_header proposed_header );

	// select lowest block hash header
	bool select_proposed_block_header( block_header& selected_header );

	// add vote to block header
	bool vote_block_header_for( int peer_id, block_header vote_header );

	// get elected block header ( +1/2 votes )
	bool get_elected_block_header( block_header& elected_header );

	// verify block
	bool add_verified_block( int peer_id, block verified_block );

	// find committed block
	bool find_commit_block( block& commit_block );
};

