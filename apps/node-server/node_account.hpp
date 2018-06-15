#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

// node account class with public key, private key
class node_account
{
	using key_pair = bryllite::key_pair;
public:
	key_pair _key_pair;

	node_account();

	// account address
	std::string address( void );

	uint264 public_key( void );
	uint256 private_key( void );

	bool load_key_file( std::string keyfile );
	bool save_key_file( std::string keyfile );

	void generate_new_key( void );
};


