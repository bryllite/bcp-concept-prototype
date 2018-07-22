#pragma once

// public key class
class CPublicKey : public uint264
{
protected:
	CAddress _address;		// address

public:
	CPublicKey();
	CPublicKey( const uint264& key );
	CPublicKey( std::string key );

	operator uint264&() const;
	operator uint264() const;

	std::string str( void );

	CAddress& address( void );

protected:
	CAddress compute_address( void ) const;
};

// make account address from the public key ( base58_encode( RIPEMD160( sha256(public_key) ) + checksum(4) ) )
std::string public_key_to_address( uint264 public_key );
