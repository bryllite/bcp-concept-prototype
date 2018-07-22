#include "stdafx.hpp"
#include "node_account.hpp"

node_account::node_account() 
{
};

// account address
std::string node_account::address( void ) 
{
	return _key_pair._public.address();
};

uint264 node_account::public_key( void ) 
{
	return _key_pair.get_public_key();
};

uint256 node_account::private_key( void ) 
{
	return _key_pair.get_private_key();
};

bryllite::key_pair node_account::keyPair( void )
{
	return _key_pair;
}

void node_account::generate_new_key( void )
{
	_key_pair.generate_new();
}

bool node_account::load_key_file( std::string keyFile )
{
	return _key_pair.read( keyFile );
}

bool node_account::save_key_file( std::string keyFile )
{
	return _key_pair.write( keyFile );
}

std::string node_account::to_string( void )
{
	std::stringstream ss;

	ss << "{ address=" << _key_pair.address()
		<< ", public_key=" << _key_pair.get_public_key().GetHex()
		<< ", private_key=" << _key_pair.get_private_key().GetHex()
		<< " }";

	return ss.str();
}
