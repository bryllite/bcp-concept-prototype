// secp256r1.hpp
// based on https://github.com/esxgx/easy-ecc
// author: Ken MacKay
#pragma once

#include <bryllite-common.hpp>
#include "uint256.hpp"


// namespace bryllite
namespace bryllite {

	enum 
	{
		ECC_BYTES = 32,
	};


	/* create_key_pair() function.
	Create a public/private key pair.

	Outputs:
	private_key - Will be filled in with the private key.
	public_key  - Will be filled in with the public key.

	Returns true if the key pair was generated successfully, false if an error occurred.
	*/
	bool create_key_pair( byte private_key[ECC_BYTES], byte public_key[ECC_BYTES+1] );
	bool create_key_pair( uint256& priv_key, uint264& pub_key );

	/* make_shared_secret() function.
	Compute a shared secret given your secret key and someone else's public key.
	Note: It is recommended that you hash the result of make_shared_secret before using it for symmetric encryption or HMAC.

	Inputs:
	private_key - Your private key.
	public_key  - The public key of the remote party.

	Outputs:
	secret - Will be filled in with the shared secret value.

	Returns true if the shared secret was generated successfully, false if an error occurred.
	*/
	bool make_shared_secret( const byte _private[ECC_BYTES], const byte _public[ECC_BYTES+1], byte _secret[ECC_BYTES] );
	bool make_shared_secret( const uint256 _private, const uint264 _public, uint256& _secret );

	/* do_sign() function.
	Generate an ECDSA signature for a given hash value.

	Usage: Compute a hash of the data you wish to sign (SHA-2 is recommended) and pass it in to
	this function along with your private key.

	Inputs:
	private_key - Your private key.
	hash       - The message hash to sign.

	Outputs:
	signature  - Will be filled in with the signature value.

	Returns true if the signature generated successfully, false if an error occurred.
	*/
	bool do_sign( const byte _private[ECC_BYTES], const byte _hash[ECC_BYTES], byte _signature[ECC_BYTES*2] );
	bool do_sign( const uint256 _privateKey, const uint256 _hash, uint512& _signature );

	/* do_verify() function.
	Verify an ECDSA signature.

	Usage: Compute the hash of the signed data using the same hash as the signer and
	pass it to this function along with the signer's public key and the signature values (r and s).

	Inputs:
	public_key - The signer's public key
	hash      - The hash of the signed data.
	signature - The signature value.

	Returns true if the signature is valid, false if it is invalid.
	*/
	bool do_verify( const byte _public[ECC_BYTES+1], const byte _hash[ECC_BYTES], const byte _signature[ECC_BYTES*2] );
	bool do_verify( const uint264 _publicKey, const uint256 _hash, const uint512 _signature );

};//namespace bryllite
