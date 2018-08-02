#pragma once

// roundID
typedef size_t RoundID;

// bcp step
enum class bcp_steps
{
	new_round,
	propose,
	vote,
	verify,
	commit,
};

#if defined(_DEBUG) //&& 0
#define _USE_SHORT_BLOCK_TIME_
#endif


// bcp timeout
enum bcp_timeout
{
#ifdef _USE_SHORT_BLOCK_TIME_
	proof_of_participation = 2500,
	propose = 5000,
	vote = 7500,
	commit = 10000,
#else // _USE_SHORT_BLOCK_TIME_
	proof_of_participation = 5000,
	propose = 10000,
	vote = 20000,
	commit = 30000,
#endif // _USE_SHORT_BLOCK_TIME_
	block_time = commit
};


// BCP round data
class CBcpRoundData
{
	friend class CBcpWorker;
protected:

	// round number (=BlockID)
	RoundID _round;

	// round begin time stamp
	time_t _begin_time;

	// current step
	bcp_steps _step;

public:
	// ctor
	CBcpRoundData(RoundID round, time_t beginTime);

	// round
	RoundID round(void);

	// step accessor
	void step(bcp_steps step);
	bcp_steps step(void);

	// begin time accessor
	time_t beginTime(void);
	void beginTime(time_t bTime);


protected:
	/////////////////////////////////
	// 1. proof of participation
	/////////////////////////////////

	// node signed block
	CBlock _node_signed_block;

	// user signed headers ( using block reward distribution to game user )
	std::map< CAddress, CBlockHeader > _user_signed_headers;

	// lowest user signed header
	CBlockHeader _user_signed_header;

public:
	// node signed block
	void node_signed_block(CBlock block);
	CBlock& node_signed_block(void);

	// append user signed header
	bool append_user_signed_headers(CAddress userAddress, CBlockHeader userHeader);

	// select lowest user signed block header
	bool select_user_signed_headers(CBlockHeader& header);


protected:
	//////////////////////////////
	// 2. propose step
	//////////////////////////////

	// proposed headers
	std::map< NodeID, CBlockHeader > _proposed_headers;

	// header selected by propose 
	CBlockHeader _proposed_header;

public:

	// append propose header from other peer
	bool append_proposed_headers(NodeID nodeId, CBlockHeader proposedHeader);

	// select lowest header
	bool select_proposed_headers(CBlockHeader& header);



	//////////////////////////////
	// 3. vote step
	//////////////////////////////
protected:
	// voted headers
	std::map< NodeID, CBlockHeader > _voted_headers;
	std::map< uint256, CBlockHeader > _hashed_voted_headers;
	std::map< uint256, size_t > _hashed_votes;

	// header selected by vote ( automatically set by find_vote_qualified_header() )
	CBlockHeader _voted_header;

public:
	// append peer voted header
	bool append_voted_headers(NodeID nodeId, CBlockHeader voteHeader);

	// find qualified voted header
	bool find_vote_qualified_header(size_t qualifiedCount, CBlockHeader& qualifiedHeader);

	// get vote qualified header
	CBlockHeader& get_vote_qualified_header(void);



	//////////////////////////////
	// 4. commit step
	//////////////////////////////

	// candidate block ( need to verify )
	CBlock _candidate_block;

	// commit completed blocks
	std::map< NodeID, CBlock > _commit_blocks;

	// found new block
	CBlock _new_block;
};


// node server
class CNodeServer;

// bcp processor
class CBcpWorker final
{
public:
	enum { invalid_round = -1 };

private:
	// node server ref.
	CNodeServer& _node_server;

private:
	// time context
	time_t _time_context;

	// current round
	RoundID _current_round;

	// new round messages
	std::map< int, size_t > _new_round_messages;
	std::map< size_t, size_t > _new_round_votes;

	// bcp round data map
	std::map< RoundID, std::unique_ptr<CBcpRoundData> > _bcp_round_data_map;

public:
	// ctor
	CBcpWorker(CNodeServer& nodeServer);

	// do bcp process
	int poll(void);

protected:
	time_t getTimeLine(time_t& timeContext);
	time_t getTimeLine(void);

	CBcpRoundData* getRoundData(RoundID round);
	CBcpRoundData* getRoundData(void);

	// is new round started?
	bool new_round_qualified(RoundID& round);
	int process_new_round(void);



protected:
	int onNewRoundMessage(NodeID nodeId, node_message_new_round* msg);
	int onUserBlockMessage(message_header_sign_ack* msg);
	int onProposeMessage(NodeID nodeId, node_message_propose* msg);
	int onVoteMessage(NodeID nodeId, node_message_vote* msg);
	int onVerifyMessage(NodeID nodeId, node_message_verify_block* msg);
	int onCommitMessage(NodeID nodeId, node_message_commit* msg);
};
