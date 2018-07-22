#pragma once

// node account class with public key, private key
class node_account
{
	using key_pair = bryllite::key_pair;
protected:
	key_pair _key_pair;

public:
	node_account();

	// account address
	std::string address( void );

	// public/private key
	uint264 public_key( void );
	uint256 private_key( void );

	key_pair keyPair( void );

	// key file
	bool load_key_file( std::string keyfile );
	bool save_key_file( std::string keyfile );

	// generate new account
	void generate_new_key( void );

	// node_account to string
	std::string to_string( void );
};


