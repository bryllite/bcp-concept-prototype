#pragma once

// BCP Round Index(=BlockIndex)
typedef int RoundIdx;
const int INVALID_ROUND = -1;

// BCP steps
enum class BcpStep
{
	Waiting,
	ProofOfParticipation,
	Propose,
	Vote,
	Commit
};

#if defined(_DEBUG) && 0
#define __USE_SHORT_BLOCK_TIME__
#endif

// Bcp timeout
enum BcpTimeout
{
#ifdef __USE_SHORT_BLOCK_TIME__
	ProofOfParticipation = 2500,
	Propose = 5000,
	Vote = 7500,
	Commit = 10000,
#else//__USE_SHORT_BLOCK_TIME__
	ProofOfParticipation = 5000,
	Propose = 10000,
	Vote = 20000,
	Commit = 30000,
#endif//__USE_SHORT_BLOCK_TIME__
	BlockTime = Commit
};
