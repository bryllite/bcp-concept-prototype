#include <cstdio>
#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include <openssl/x509v3.h>
#include <openssl/ecdsa.h>
#include <openssl/pem.h>

typedef struct _u256 {
	uint8_t _data[32];

	_u256() {
		memset( _data, 0, sizeof(_data) );
	};
} u256;



// secp256k1 class
class secp256k1
{
public:
	secp256k1_context* _secp256k1_context;

	secp256k1()
	{
		srand( time( 0 ) );

		_secp256k1_context = secp256k1_context_create( SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY );

		uint8_t seed[32];
		random_bytes( seed, 32 );
		secp256k1_context_randomize( _secp256k1_context, seed );
	};

	~secp256k1()
	{
		if ( _secp256k1_context ) {
			secp256k1_context_destroy( _secp256k1_context );
		}
	};

protected:
	void random_bytes( uint8_t* buff, size_t len )
	{
		for( size_t i = 0 ; i < len ; i++ )
			buff[i] = (uint8_t)rand();
	};

public:

	// create private key
	bool ecc_create_private_key( uint8_t* private_key, size_t len )
	{
		_assert( private_key != nullptr, "private_key is null. len=%d", len );

		do {
			random_bytes( private_key, 32 );
		} while( 0 == ecc_verify_private_key( private_key ) );

		return true;
	};

	// get public key for private key
	bool ecc_get_public_key( const uint8_t* private_key, uint8_t* public_key, size_t* len, bool compressed )
	{
		secp256k1_pubkey pubkey;
		_assert( *len == (compressed?33:65), "public key size mismatch. len=%d, compressed=%s", *len, compressed?"true":"false" );
		memset( public_key, 0, *len );

		if ( !secp256k1_ec_pubkey_create( _secp256k1_context, &pubkey, (const unsigned char*)private_key ) ) {
			return false;
		}

		if ( !secp256k1_ec_pubkey_serialize( _secp256k1_context, public_key, len, &pubkey, compressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED ) ) {
			return false;
		}

		return true;
	};

	// verify private key
	bool ecc_verify_private_key( const uint8_t* private_key )
	{
		return secp256k1_ec_seckey_verify( _secp256k1_context, (const unsigned char*)private_key );
	};

	// sign
	bool ecc_sign( const uint8_t* private_key, const u256 hash, unsigned char* sigder, size_t* outlen ) 
	{
		secp256k1_ecdsa_signature sig;

		if ( !secp256k1_ecdsa_sign( _secp256k1_context, &sig, (const unsigned char*)&hash, private_key, secp256k1_nonce_function_rfc6979, nullptr ) ) {
			return false;
		}

		if ( !secp256k1_ecdsa_signature_serialize_der( _secp256k1_context, sigder, outlen, &sig ) ) {
			return false;
		}

		return true;
	};

	// verify sig
	bool ecc_verify_sig( const uint8_t* public_key, bool compressed, const u256 hash, unsigned char* sigder, size_t siglen )
	{
		secp256k1_ecdsa_signature sig;
		secp256k1_pubkey pubkey;

		if ( !secp256k1_ec_pubkey_parse( _secp256k1_context, &pubkey, public_key, compressed ? 33 : 65 ) ) {
			return false;
		}

		if ( !secp256k1_ecdsa_signature_parse_der( _secp256k1_context, &sig, sigder, siglen ) ) {
			return false;
		}

		return secp256k1_ecdsa_verify( _secp256k1_context, &sig, ( const unsigned char* )&hash, &pubkey );
	};
};


// openssl test
int main( int argc, char** argv ) 
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	log( "Hello, Bryllite! %s", bryllite::version() ) ;

	OPENSSL_init_crypto( 0, nullptr );

	EVP_PKEY* pkey = EVP_PKEY_new();
	EC_KEY* ecc = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
	if ( !ecc ) {
		elog( "OpenSSL Error. EC_KEY_new_by_curve_name() failed" );
		return 1;
	}

	EC_KEY_set_asn1_flag(ecc, OPENSSL_EC_NAMED_CURVE );
	if ( EC_KEY_generate_key(ecc) < 0 ) {
		elog( "OpenSSL Error. EC_KEY_generate_key() failed" );
		return 1;
	}

	if ( EVP_PKEY_assign_EC_KEY( pkey, ecc ) < 0 ) {
		elog( "OpenSSL Error. EVP_PKEY_assign_EC_KEY() failed" );
		return 1;
	}

	const char* export_password = "rx78gp03";

	FILE* fp = fopen( "private_key.pem", "wb" );
	if ( fp )
	{
		if ( PEM_write_PrivateKey( fp, pkey, EVP_des_ede3_cbc(), (unsigned char*)export_password, (int)strlen(export_password), nullptr, nullptr ) < 0 ) {
			elog( "OpenSSL Error, PEM_write_PrivateKey() failed" );
			return 1;
		}

		fclose(fp);
	}



	return 0 ;
}

