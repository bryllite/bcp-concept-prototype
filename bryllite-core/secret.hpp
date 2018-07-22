#pragma once

// Secret class
class CSecret
{
protected:
	uint264 _key;			// secret key
	uint512 _signature;		// signature

public:
	// ctor
	CSecret();
	CSecret( CKeyPair keyPair, uint256 hash );
	CSecret( CKeyPair keyPair, std::string hash );

	// is null?
	bool isNull( void ) const;

	// sign
	bool sign( CKeyPair keyPair, uint256 hash );
	bool sign( CKeyPair keyPair, std::string hash );

	// verify
	bool verify( uint256 hash ) const;
	bool verify( std::string hash ) const;

	// hash
	uint256 hash( void ) const;

	// compare
	bool operator==( const CSecret& other ) const;
	bool operator!=( const CSecret& other ) const;

	// owner address
	CAddress owner( void ) const;

	// to_string
	std::string to_string( void ) const;
};
