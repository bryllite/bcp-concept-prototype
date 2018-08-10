#pragma once

//NAMESPACE_BEGIN(bryllite)
//NAMESPACE_BEGIN(bcp)


// bcp host class
class CBcpHost
{
protected:
	// bcp timer ref
	CBcpTimer & _bcp_timer_ref;

	// blcok chain ref
	CBlockChain& _block_chain_ref;

	// is stopped?
	bool _stopped;

	// round provider
	CRoundProvider _round_provider;

	// round helpers
	std::map< RoundIdx, std::unique_ptr<CRoundHelper> > _round_helpers;

public:
	// bcp host ctor
	CBcpHost(CBcpTimer& bcpTimer, CBlockChain& blockChain);

	// start/stop/update
	bool start(void);
	void stop(void);
	int update(void);

	// is stopped?
	bool stopped(void) const;

protected:
	// get round helper ( default: current round helper )
	CRoundHelper& getRoundHelper(RoundIdx roundIdx = INVALID_ROUND) const;

	// step processor
protected:
	int onUpdateNewRound(void);
	int onUpdateProofOfParticipation(CRoundHelper& roundHelper);
	int onUpdatePropose(CRoundHelper& roundHelper);
	int onUpdateVote(CRoundHelper& roundHelper);
	int onUpdateCommit(CRoundHelper& roundHelper);


	// BCP callback interface
protected:
	virtual size_t qualifyVotes(void) = 0;
	virtual int onBcpNewRoundMessage(RoundIdx newRound) = 0;
	virtual int onBcpProofOfParticipationStart(CRoundHelper& roundHelper) = 0;
	virtual int onBcpProposeStart(CRoundHelper& roundHelper, const CBlockHeader& header) = 0;
	virtual int onBcpVoteStart(CRoundHelper& roundHelper, const CBlockHeader& header) = 0;
	virtual int onBcpCommitStart(CRoundHelper& roundHelper, const CBlockHeader& header) = 0;
	virtual int onBcpCommit(CRoundHelper& roundHelper, const CBlock& block) = 0;
	virtual int onBcpNewBlock(CRoundHelper& roundHelper, const CBlock& newBlock) = 0;
	virtual int onBcpAbandoned(CRoundHelper& roundHelper) = 0;
	virtual int onBcpRoundCompleted(CRoundHelper& roundHelper, const CBlock& newBlock) = 0;
};



//NAMESPACE_END(bcp)
//NAMESPACE_END(bryllite)
