#pragma once

//NAMESPACE_BEGIN(bryllite)
//NAMESPACE_BEGIN(bcp)

// round provider class
class CRoundProvider
{
protected:
	// bcp timer
	CBcpTimer& _bcp_timer_ref;

	// block chain
	CBlockChain& _block_chain_ref;

	// time context
	time_t _time_context;

	// current round
	RoundIdx _current_round;

	// new round messages
	std::map< NodeID, RoundIdx > _new_round_messages;
	std::map< RoundIdx, size_t > _new_round_votes;


public:
	// round provider ctor
	CRoundProvider(CBcpTimer& bcpTimer, CBlockChain& blockChain);

	// clear new round messages
	void clear_new_round_messages(void);

	// current round
	RoundIdx currentRound(void) const;
	void currentRound(RoundIdx roundIdx);

	// next round
	RoundIdx nextRound(void) const;

	// get bcp timeline with timeContext
	time_t getTimeLine(time_t& timeContext) const;
	time_t getTimeLine(void) const;


	// ready for new round
	bool ready_for_new_round_message(RoundIdx& newRound);

	// new round started?
	bool new_round_started(size_t qualifyVotes, RoundIdx& newRound);

	// append new round message
	bool append_new_round_message(NodeID nodeId, RoundIdx newRound);

};

//NAMESPACE_END(bcp)
//NAMESPACE_END(bryllite)
