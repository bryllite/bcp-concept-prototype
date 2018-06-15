#pragma once

#include <bryllite-common.hpp>
#include "uint256.hpp"
#include "key.hpp"

// namespace bryllite
namespace bryllite {

class transaction
{
	struct data
	{
		time_t _timestamp;
		address _from;
		address _to;
		size_t _amount;	

		data();
		data( address from, address to, size_t amount );
		data( std::string from, std::string to, size_t amount );
	};

public:
	data _data;
	uint264 _publicKey;
	uint512 _signature;

	transaction();
	transaction( address from, address to, size_t amount );
	transaction( std::string from, std::string to, size_t amount );

	// is owner & valid signature?
	// need to check owner balance outside transaction
	bool valid( void );

	bool sign( uint256 priv_key, uint264 pub_key );

	std::string dump( void );

	size_t serialize( byte* target, size_t target_len );
	size_t deserialize( byte* src, size_t src_len );
};

using transactions = std::vector< transaction >;

// get transaction id
std::string transaction_id( transaction tx );

};//namespace bryllite
