#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>


// list of block
class block_chain : public bryllite::lockable
{
	using block = bryllite::block;
public:
	std::vector< block > _blocks;
	std::map< std::string, uint64_t > _accountBalances;

public:
	block_chain();

	size_t block_height( void );

	// verify block
	bool verify_block( block block_ );

	// append block
	bool append_block( block block_ );

	// update balance
	bool update_balance( block block_ );

	// get block
	block get_block( size_t blockNumber );
	bool get_block( size_t block_index, block& b );

	// get balance of account
	uint64_t balanceOf( std::string account );

};

