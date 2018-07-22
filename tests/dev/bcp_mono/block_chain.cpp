#include "block_chain.hpp"

block_chain::block_chain() : _last_block_index(0)
{
}

block_chain::block_chain( std::string filename ) : block_chain()
{
}

size_t block_chain::size( void )
{
	return _blocks.size();
}


// append block
bool block_chain::append_block( size_t block_index, block& b, bool build_balance )
{
	smartlock(this);

	// check existing block
	if ( _blocks.count( block_index ) > 0 ) return false;

	// insert block 
	_blocks.insert( std::make_pair( block_index, b ) );

	// last block index
	if ( _last_block_index < block_index ) _last_block_index = block_index;

	// build account balance
	if ( build_balance ) {
		build_balance_for(b);
	}

	return true;
}

// get block
bool block_chain::get_block( size_t block_index, block& b )
{
	if ( _blocks.count( block_index ) > 0 ) {
		b = _blocks[block_index];
		return true;
	}

	return false;
}


// get balance of address
uint64_t block_chain::balanceOf( bryllite::address addr )
{
	smartlock(this);

	if ( _balances.count( addr ) > 0 ) {
		return _balances[addr];
	}

	return 0;
}

bool block_chain::rebuild_balance( void )
{
	smartlock(this);

	for( auto& pair : _blocks )
	{
		size_t block_index = pair.first;
		block& b = pair.second;

		build_balance_for(b);
	}

	return true;
}

bool block_chain::build_balance_for( block& b )
{
	smartlock(this);

	for( auto& tx : b._transactions ) 
	{
		address from = tx.from();
		address to = tx.to();
		uint64_t value = tx.value();

		if ( value == 0 ) continue ;

		if ( !from.empty() && from != to )
		{
			std::string s_from = from.to_string();
			if ( _balances.count( s_from ) > 0 && _balances[s_from] >= value ) {
				_balances[ s_from ] -= value;
			} 
			else 
			{
				wlog( "not enough balance for '%s'", from );
				continue;
			}
		}

		if ( !to.empty() )
		{
			std::string s_to = to.to_string();
			if ( _balances.count( s_to ) > 0 ) {
				_balances[ s_to ] += value;
			} 
			else 
			{
				_balances[ s_to ] = value;
			}
		}
	}

	return true;
}


