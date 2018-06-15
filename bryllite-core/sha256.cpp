#include "sha256.hpp"

// namespace bryllite
namespace bryllite {


// sha256 hash 
void sha256( byte* target, size_t target_len, byte* hash, size_t hash_len )
{
	unsigned char _hash[SHA256_DIGEST_LENGTH];

	SHA256_CTX _sha256;
	SHA256_Init( &_sha256 );
	SHA256_Update( &_sha256, target, target_len );
	SHA256_Final( _hash, &_sha256 );

	memcpy( hash, _hash, hash_len < SHA256_DIGEST_LENGTH ? hash_len : SHA256_DIGEST_LENGTH );
};

// sha256 hash
std::string sha256_hex_string( std::string str )
{
	return hash256_hex_string( str );
};

// sha256
std::string sha256_hex_string( byte* data, size_t data_len )
{
	return hash256_hex_string( data, data+data_len );
};

uint256 sha256( std::string str )
{
	uint256 hash;
	hash.SetHex( hash256_hex_string( str ) );
	return hash;
};

uint256 sha256( byte* data, size_t data_len )
{
	uint256 hash ;
	sha256( data, data_len, (byte*)&hash, sizeof(hash) );
	return hash;
};


};//namespace bryllite
