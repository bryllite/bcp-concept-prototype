#pragma once


// brc coin class
class CCoin 
{
protected:
	uint64_t _value;

public:
	CCoin();
	CCoin(const uint64_t value);

	operator uint64_t(void);
	operator uint64_t&(void);

	std::string brc(void);
	std::string bryl(void);
};


// 1 BRC = 100000000 bryl
enum { brc_decimals = 8 };
static const char* brc_symbol = "BRC";
static const char* bryl_symbol = "bryl";
static const uint64_t default_block_reward = 100;

#ifdef _DEBUG
static const uint64_t halving_block = 1024;
#else
static const uint64_t halving_block = 1051200;
#endif


// bryl per 1 BRC ( 10^brc_decimals )
double bryl_per_brc(void);

// get block reward for block height
uint64_t block_reward_for(size_t block_height);


// convert bryl to BRC
double bryl_to_brc(uint64_t bryl);

// to BRC string
std::string toBRC(uint64_t bryl);
