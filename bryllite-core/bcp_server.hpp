#pragma once


// bcp callback interface
class IBcpServerCallback
{
public:
	virtual size_t qualify_votes_count( void ) = 0;

	virtual int onBcpNewRoundReady( size_t prevRoundIdx ) = 0;
	virtual int onBcpNewRoundStart( size_t roundIdx ) = 0;
	virtual int onBcpProposeStart( size_t roundIdx ) = 0;
	virtual int onBcpVoteStart( size_t roundIdx ) = 0;
	virtual int onBcpCommitStart( size_t roundIdx, CBlockHeader vote_header ) = 0;
	virtual bool onBcpVerifyBlock( size_t roundIdx, CBlock verify_block ) = 0;

	virtual int onBcpNewBlock( size_t roundIdx, CBlock newBlock ) = 0;
	virtual int onBcpTimeout( size_t roundIdx ) = 0;
};

class CBcpServer;

// round storage
class CBcpData 
{
	friend class CBcpServer;
protected:

	// lock objects of CBcpServer
	bryllite::lockable& _lock;

	// current block idx
	size_t _round_idx;

	// node candidate block
	CBlock _candidate_block;

	// user signed headers
	std::map< std::string, CBlockHeader > _user_signed_headers;
	CBlockHeader _lowest_user_signed_header;

	// node proposed headers
	std::map< int, CBlockHeader > _proposed_headers;
	CBlockHeader _lowest_proposed_header;

	// node voted headers
	std::map< int, CBlockHeader > _votes_box;
	std::map< uint256, CBlockHeader > _votes_headers;
	std::map< uint256, size_t > _votes;
	CBlockHeader _votes_qualified_header;
	bool _votes_completed;

	// verify block
	CBlock _verify_block;
	bool _verify_completed;

	// node committed block
	CBlock _commit_block;
	std::map< int, CBlock > _committed_blocks;
	std::map< int, CSecret > _committed_secrets;
	bool _commit_completed;

public:
	CBcpData( size_t roundIdx, bryllite::lockable& lock );

	size_t round_idx( void );

	// candidate block
	void candidate_block( CBlock block );
	CBlock& candidate_block( void );

	// user signed header
	bool append_user_signed_header( std::string user_address, CBlockHeader signed_header );
	bool find_lowest_user_signed_header( CBlockHeader& lowest_header );
	bool get_lowest_user_signed_header( CBlockHeader& lowest_header );
	size_t get_participated_users( std::vector< std::string >& users );

	// proposed header
	bool append_proposed_header( int peer_id, CBlockHeader proposed_header );
	bool find_lowest_proposed_header( CBlockHeader& lowest_header );
	bool get_lowest_proposed_header( CBlockHeader& lowest_header );

	// votes headers
	bool append_votes_header( int peer_id, CBlockHeader votes_header );
	bool find_qualified_votes_header( CBlockHeader& qualified_header, size_t qualify_count );
	bool get_qualified_votes_header( CBlockHeader& qualified_header );

	// commit block
	bool append_commit_block( int peer_id, CBlock commit_block, CSecret commit_secret );
	bool find_qualified_commit_block( CBlock& qualified_block, size_t qualify_count );
	bool get_qualified_commit_block( CBlock& qualified_block );

	// verify block
	void verify_block( CBlock block );
	CBlock& verify_block( void );
};


#if defined(_DEBUG) && 0
#define _USE_SHORT_BLOCK_TIME_
#endif


// BCP class
class CBcpServer : public ICallbackTimer
{
	using callback_timer = bryllite::callback_timer;

	// bcp timeout in ms
	enum bcp_timeout
	{
#ifdef _USE_SHORT_BLOCK_TIME_
		bcp_timeout_proof_of_participation = 4000,
		bcp_timeout_propose = 6000,
		bcp_timeout_vote = 8000,
		bcp_timeout_commit = 10000,
#else // _USE_SHORT_BLOCK_TIME_
		bcp_timeout_proof_of_participation = 5000,
		bcp_timeout_propose = 10000,
		bcp_timeout_vote = 20000,
		bcp_timeout_commit = 30000,
#endif
		bcp_block_time = bcp_timeout_commit
	};

protected:
	// lock object of CNodeServer
	bryllite::lockable& _lock;

	// CBcpTimer ref
	CBcpTimer& _bcp_timer;

	// callback timer
	callback_timer _callback_timer;

	// BCP callback interface
	IBcpServerCallback& _bcp_callback;

	// time context
	time_t _time_context;

	// round idx
	size_t _round_idx;

	// new round messages
	std::map< int, size_t > _new_round_messages;
	std::map< size_t, size_t > _new_round_votes;

	// < round, CBcpData >
	std::map< size_t, std::unique_ptr< CBcpData > > _bcp_data;

public:
	CBcpServer( IBcpServerCallback& bcp_callback, bryllite::lockable& lock, CBcpTimer& bcpTimer );

	// start/stop bcp
	bool start( void );
	bool stop( void );

	// update bcp
	int update( void );

	CBcpData* get_bcp_data( size_t round_idx );
	CBcpData* get_bcp_data( void );

	bool ready( void );

public:
	time_t getTime( void );

	// timeline in (ms)
	time_t timeline( time_t& timeContext );
	time_t timeline( void );

protected:
	enum 
	{
		timeout_proof_of_participation = 0,
		timeout_propose,
		timeout_vote,
		timeout_commit
	};

	int onTimeOut(timer_id id, void* pContext) override;
	void onTimeOutProofOfParticipation( void );
	void onTimeOutPropose( void );
	void onTimeOutVote( void );
	void onTimeOutCommit( void );

	void setTimeOutProofOfParticipation( void );
	void setTimeOutPropose( void );
	void setTimeOutVote( void );
	void killTimeOutVote( void );
	void setTimeOutCommit( void );
	void killTimeOutCommit( void );

public:
	// add new round message
	bool new_round_vote( int peer_id, size_t round_idx );

	// is new round vote qualified?
	bool new_round_vote_qualified( size_t& round_idx );

	bool onVote( int peer_id, size_t round_idx, CBlockHeader vote_header );
	bool onVerify( int peer_id, size_t round_idx, CBlock verify_block );
	bool onCommit( int peer_id, size_t round_idx, CBlock commit_block, CSecret secret );

};

