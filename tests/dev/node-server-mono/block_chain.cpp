#include "stdafx.hpp"
#include "block_chain.hpp"

block_chain::block_chain() 
{
};

size_t block_chain::height( void ) 
{
	return _mapBlocks.size();
};

// get balance of account
uint64_t block_chain::balanceOf( std::string account ) 
{
	return _accountBalances.count( account ) > 0 ? _accountBalances[account] : 0;
};


// verify block
bool block_chain::verify_block( block block_ )
{
	// verify block
//	if( !block_.valid() ) return false;

	// verify block transactions
	for( auto& tx : block_._transactions )
	{
		uint64_t value = tx.value();
		std::string from = tx.from.to_string();
		std::string to = tx.to.to_string();

		// valid signatured?
		if( !tx.valid() ) return false;

		// value > 0
		if( value <= 0 ) return false;

		// enough balance for from?
//		if( balanceOf( from ) < amount ) return false;
	}

	return true;
}

// append block
bool block_chain::append_block( block block_ )
{
	smartlock( this );

	size_t block_index = block_.block_index();

	// existing block?
	if ( block_exists( block_index ) )
		return false;

	// verify block
	if( !verify_block( block_ ) )
		return false;

	// append block
	_mapBlocks.insert( std::make_pair( block_index, block_ ) );

	// update balance for block
	return update_balance( block_ );
}

// update balance
bool block_chain::update_balance( block block_ )
{
	// account balance update
	for( auto& tx : block_._transactions )
	{
		uint64_t value = tx.value();

		if ( !tx.coinbase() )
		{
			std::string from = tx.from().to_string();
			_accountBalances.count(from)>0? (_accountBalances[from] -= value) : (_accountBalances[from] = 0) ;
		}

		std::string to = tx.to.to_string();
		_accountBalances.count( to )>0 ? ( _accountBalances[to] += value ) : ( _accountBalances[to] = value );
	}

	return true;
}

// get block
bryllite::block block_chain::get_block( size_t block_index )
{
	for( auto& pair : _mapBlocks )
	{
		size_t idx = pair.first;

		if ( idx == block_index )
			return pair.second;
	}

	return block();
}

bool block_chain::get_block( size_t block_index, block& b )
{
	for( auto& pair : _mapBlocks )
	{
		size_t idx = pair.first;

		if ( idx == block_index )
		{
			b = pair.second;
			return true;
		}
	}

	return false;
}

bool block_chain::block_exists( size_t block_index )
{
	return _mapBlocks.count( block_index ) > 0 ;
}

// get block header hash
uint256 block_chain::get_block_hash( size_t block_index )
{
	uint256 hash ;

	block b;
	if ( get_block( block_index, b ) )
		hash = b.get_header_hash();

	return hash;
}
