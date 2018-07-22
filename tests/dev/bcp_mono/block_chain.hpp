#pragma once

#include <bryllite-core.hpp>


// block_chain class
class block_chain : public bryllite::lockable
{
	using block = bryllite::block;
	using address = bryllite::address;
	using transaction = bryllite::transaction;

protected:
	std::map< size_t, block > _blocks;			// map of blocks ( indexed )
	std::map< address, uint64_t > _balances;	// account balances
	size_t _last_block_index;

public:
	block_chain();
	block_chain( std::string filename );

	// append block
	bool append_block( size_t block_index, block& b, bool build_balance );

	// get block
	bool get_block( size_t block_index, block& b );

	// get balance of address
	uint64_t balanceOf( address addr );

	// get size of
	size_t size( void );

protected:

	bool build_balance_for( block& b );
	bool rebuild_balance( void );

	// is valid transaction?
	bool is_valid_transaction( transaction& tx );

	// is valid block?
	bool is_valid_block( block& b );

	// serialize/unserialize to/from file
	bool serialize( std::string filename );
	bool unserialize( std::string filename );

};
