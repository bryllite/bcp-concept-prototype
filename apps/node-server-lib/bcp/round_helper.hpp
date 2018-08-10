#pragma once

//NAMESPACE_BEGIN(bryllite)
//NAMESPACE_BEGIN(bcp)


// user-signed header lists
class CUserSignedHeaderBox
{
protected:
	// node-generated & user-signed block
	CBlock _candidate_block;

	// user signed header lists
	std::map< CAddress, CBlockHeader > _user_headers;

	// selected(lowest) user signed header
	CBlockHeader _selected_user_header;

public:
	// default ctor
	CUserSignedHeaderBox();

	// candidate block(header)
	bool candidate(const CBlock& block);
	bool candidate(const CBlockHeader& header);
	const CBlock& candidate(void) const;

	// append user-signed block header
	bool appendUserHeader(const CAddress& address, const CBlockHeader& header);

	// select lowest header
	const CBlockHeader& selectUserHeader(void) const;

	// get participated user address lists
	size_t findParticipatedUsers(std::vector<CAddress>& users) const;

};


// proposed header box
class CProposeHeaderBox
{
protected:
	// proposed header lists
	std::map<NodeID, CBlockHeader > _proposed_headers;

	// selected(lowest) header in proposed lists
	CBlockHeader _selected_proposed_header;

public:
	// default ctor
	CProposeHeaderBox();

	// append proposed header
	bool appendProposedHeader(NodeID nodeId, const CBlockHeader& header);

	// select lowest header ( can be null )
	const CBlockHeader& selectProposedHeader(void) const;

};

// vote header box
class CVoteHeaderBox
{
protected:
	std::map< NodeID, CBlockHeader > _vote_headers;
	std::map< uint256, CBlockHeader > _hashed_headers;
	std::map< uint256, size_t > _votes;

	CBlockHeader _elected_header;

public:
	// default ctor
	CVoteHeaderBox();

	// append voted header
	bool appendVoteHeader(NodeID nodeId, const CBlockHeader& header);

	// select votes qualified header
	bool electedHeader(size_t qualifyVotes, CBlockHeader& header);

	// get elected header
	const CBlockHeader& electedHeader(void) const;

};

// commit block box
class CCommitBlockBox
{
protected:
	// need to verify block
	CBlock _verify_block;
	bool _verified;

	// committed block lists
	std::map< NodeID, CBlock > _commit_blocks;

	// node verify secrets
	std::map< NodeID, CSecret > _secrets;

	// committed new block
	CBlock _new_block;

public:
	CCommitBlockBox();

	// verify block
	void verifyBlock(const CBlock& block);
	const CBlock& verifyBlock(void) const;

	bool verified(void) const;
	void verified(bool verified);

	// append committed block
	bool appendCommitBlock(NodeID nodeId, const CBlock& block, const CSecret& secret);

	// find new block
	bool findNewBlock(size_t qualifyVotes, CBlock& newBlock);

	const CBlock& newBlock(void) const;

};

// BCP Round Helper
class CRoundHelper : public CUserSignedHeaderBox, public CProposeHeaderBox, public CVoteHeaderBox, public CCommitBlockBox
{
protected:
	// round idx
	RoundIdx _round_idx;

	// round begin time
	time_t _round_begin_time;

	// current bcp step
	BcpStep _bcp_step;

public:
	// default ctor
	CRoundHelper();
	// round helper ctor
	CRoundHelper(RoundIdx roundIdx, time_t roundBeginTime);

	// roundIdx, roundBeginTime
	RoundIdx roundIdx(void) const;
	time_t roundBeginTime(void) const;

	// current step
	BcpStep bcpStep(void) const;
	void bcpStep(BcpStep bcpStep);

	// operator bool
	explicit operator bool() const;
};

// null round helper
extern CRoundHelper NullRoundHelper;


//NAMESPACE_END(bcp)
//NAMESPACE_END(bryllite)

