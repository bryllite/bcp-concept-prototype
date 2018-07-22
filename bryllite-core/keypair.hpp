#pragma once

// key pair class
class CKeyPair
{
	// signature
	const char* KEY_SIGNATURE = "Bryllite";
	enum { KEY_SIGNATURE_LEN = 8 };
protected:
	// public key
	CPublicKey _publicKey;

	// private key
	CPrivateKey _privateKey;

public:
	// ctor
	CKeyPair();
	CKeyPair( const uint264& publicKey, const uint256& privateKey );
	CKeyPair( const std::string& keyFile );

	// generate new key pair
	bool newKeyPair( void );
	bool newKeyPair( CPublicKey& public_key, CPrivateKey& private_key );

	// read/write key pair
	bool loadFile( std::string keyFile );
	bool saveFile( std::string keyFile );

	CPublicKey& publicKey( void );
	CPrivateKey& privateKey( void );
	CAddress& address( void );

	// is valid keypair?
	bool isValidKey( void );

	std::string to_string( void );
};
