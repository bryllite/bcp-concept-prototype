#include <cstdio>
#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include "ripemd160.hpp"
#include "base58.hpp"




// openssl test
int main( int argc, char** argv ) 
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	log( "Hello, Bryllite! %s", bryllite::version() ) ;

	std::map< std::string, uint264 > keyMap;

	unsigned int idx = 0, collision = 0;
	bryllite::timer ts;
	while( true )
	{
		uint256 private_key;
		uint264 public_key;

		// create key pair
		bryllite::create_key_pair( private_key, public_key );

		// key string
		std::string key = public_key.GetHex() ;

		if ( keyMap.count( key ) > 0 ) 
		{
			std::cout << "[ERROR]!!: key collision! public_key=" << key << std::endl;
			collision++ ;
		} else {
			keyMap.insert( std::make_pair( key, public_key ) );
		}

		if ( ++idx % 100 == 0 ) {
			std::cout << "[" << idx << "] elapsed time=" << ts.laptime() << "(ms) keyCnt=" << keyMap.size() << ", collision=" << collision << std::endl;
		}
	};

	std::cout << "[" << idx << "] elapsed time=" << ts.laptime() << "(ms) keyCnt=" << keyMap.size() << ", collision=" << collision << std::endl;
	return 0;

#if 0
	uint256 private_key, private_key2 ;
	uint264 public_key, public_key2 ;
	uint512 signature, signature2 ;
	uint256 hash = bryllite::sha256( "Hello, Bryllite" ) ;
	uint256 hash2 = bryllite::sha256( "Hello, Bryllite! 0.1.0" );
	
	// create key pair
	bryllite::create_key_pair( private_key, public_key );
	bryllite::create_key_pair( private_key2, public_key2 );

	// sign
	bryllite::do_sign( private_key, hash, signature );
	bryllite::do_sign( private_key2, hash2, signature2 );

	bryllite::timer ts;
	for( int i=0; i<10000; i++ )
	{
		if ( !bryllite::do_verify( (byte*)&public_key, (byte*)&hash, (byte*)&signature ) )
		{
			std::cout << "Error! verified failed for signature!!" << std::endl;
			break ;
		}

		if ( i % 100 ==0 ) {
			std::cout << "[" << i << "] time verified ok" << std::endl ;
		}
	}
	std::cout << "elpased time=" << ts.laptime() << "(ms)" << std::endl;


	return 0;

//	public_key.corrupt();
//	public_key2.corrupt();

	signature.corrupt();
	signature2.corrupt();

//	hash.corrupt();
//	hash2.corrupt();

	// verify
	bool ret1 = bryllite::do_verify( public_key, hash, signature );
	bool ret2 = bryllite::do_verify( public_key2, hash2, signature2 );

	std::cout << "[ step 1 ] " << std::endl;
	std::cout << "private key: " << private_key.GetHex() << std::endl;
	std::cout << "public key: " << public_key.GetHex() << std::endl;
	std::cout << "signature: " << signature.GetHex() << std::endl;
	std::cout << "hash: " << hash.GetHex() << std::endl;
	std::cout << "verify=" << ret1 << std::endl;

	std::cout << "[ step 2 ] " << std::endl;
	std::cout << "private key: " << private_key2.GetHex() << std::endl;
	std::cout << "public key: " << public_key2.GetHex() << std::endl;
	std::cout << "signature: " << signature2.GetHex() << std::endl;
	std::cout << "hash: " << hash2.GetHex() << std::endl;
	std::cout << "verify=" << ret2 << std::endl;


	return 0;
#endif

#if 0
	bryllite::key_pair key;
	bryllite::signature _signature;

	if ( !key.generate() ) {
		elog( "ecc_make_key() failed" );
		return 0;
	}

//	byte msg_hash[bryllite::SHA256_HASH_LENGTH];
//	std::stringstream ss;

/*
	std::string priv_key = key._private.toString();
	std::string pub_key = key._public.toString();

	log( "Private Key(%d): %s", priv_key.length(), priv_key.c_str() );
	log( "Public Key(%d): %s", pub_key.length(), pub_key.c_str() );

	message_travel msg( rand() );
	bryllite::sha256( (byte*)&msg, sizeof(msg), msg_hash, sizeof(msg_hash) );
	for( size_t i=0; i< bryllite::SHA256_HASH_LENGTH; i++ )
		ss << bryllite::format( "%02x", msg_hash[i] );

	bryllite::do_sign( key._private, msg_hash, _signature );
	std::cout << "message hash: " << ss.str() << std::endl;
	std::cout << "signature: " << _signature.toString() << std::endl ;

	int ret = bryllite::do_verify( key._public, msg_hash, _signature );
	std::cout << "verify: " << (ret==0?"false":"true") << std::endl;

	ss.clear();
*/

	std::string msg = "hi this is test message" ;
	unsigned char _sha32[32];
	bryllite::sha256( (byte*)msg.data(), msg.length(), (byte*)_sha32, sizeof(_sha32) );
	std::cout << "sha256 hash: " << bryllite::toHexCode( _sha32, sizeof(_sha32) ) << std::endl ;

	std::cout << "sha256: " << bryllite::sha256_hex_string(msg) << std::endl;

	std::string address = key._public.address();
	std::cout << "address: " << address << ", len=" << address.length() << std::endl;

	uint256 from;
	from.SetHex( address );
	std::cout << "from: " << from.GetHex() << std::endl;

	uint264 test ;
	test.SetHex( key._public.toString() );

	std::cout << "test: " << test.GetHex() << std::endl ;

	using block_header = bryllite::block_header;
	using transaction = bryllite::transaction;
	using block = bryllite::block;


	bryllite::public_key pubkey( key._public.toString() );
	bryllite::private_key privkey( key._private.toString() );

	// accout address
	std::cout << "Address: " << key._public.address() << ", len=" << key._public.address().length() << std::endl;
	std::cout << "public key: " << key._public.toString() << std::endl;
	std::cout << "public key: " << pubkey.toString() << std::endl;
	std::cout << "private key: " << key._private.toString() << std::endl;
	std::cout << "private key: " << privkey.toString() << std::endl;
	std::cout << "sha256: " << bryllite::sha256_hex_string(key._public._data, sizeof(key._public._data)) << std::endl;

	std::cout << "sizeof(block_header): " << sizeof(block_header) << std::endl;
	std::cout << "sizeof(transaction): " << sizeof(transaction) << std::endl;

	transaction tx;
	tx._data._timestamp = bryllite::timestamp();
	tx._data._from.SetHex( key._public.address() );
	tx._data._to.SetHex( key._public.address() );
	tx._data._amount = 100 ;
	tx.sign( key._private.toString(), key._public.toString() );

	std::cout << "transaction_id=" << bryllite::transaction_id(tx) << std::endl;

	block_header header;
	header._header._blockNumber = rand()%100;
	header._header._timestamp = bryllite::timestamp();
	byte _hash[32];
	bryllite::sha256( (byte*)&header._header, sizeof(header._header), _hash, sizeof(_hash) );
	bryllite::do_sign( key._private._data, _hash, (byte*)&header._userSignature );
	key._public.toBytes( ( byte* )&header._userPublicKey, sizeof( header._userPublicKey ) );

	block blk;
	std::cout << "block.merkle_hash: " << blk._header._header._hashMerkleRoot.GetHex() << std::endl;
	blk.append_transaction( tx );
	std::cout << "block.merkle_hash: " << blk._header._header._hashMerkleRoot.ToString() << std::endl;

	std::cout << "block_header.valid()?" << (header.valid()?"true":"false") << std::endl;


/*
	// public key to address
	std::cout << "[public key -> address]" << std::endl;
	std::cout << "public key: " << key._public.toString() << std::endl;

	// public key to sha256
	byte hash[ 32 ];
	bryllite::sha256( key._public._key, sizeof( key._public._key ), hash, sizeof(hash) );
	std::cout << "public key (SHA256): " << bytesToString( hash, sizeof(hash) ) << std::endl;

	// sha256 -> ripemd160 ( 20 byte )
	byte _hash160[ 20 ];
	ripemd160( hash, sizeof( hash ), _hash160 );
	std::cout << "public key (ripemd160): " << bytesToString( _hash160, sizeof(_hash160) ) << std::endl;

	// encode base58
	std::string base58 = EncodeBase58( _hash160, _hash160+sizeof(_hash160) );
	std::cout << "public key (base58): " << base58 << std::endl;

	// decode base58
	std::stringstream ss;
	std::vector< unsigned char > _base58;
	byte _decode[20];
	memset( _decode, 0, sizeof(_decode) );
	DecodeBase58( base58, _base58 );
	int i=0;
	for( auto& c : _base58 )
		_decode[i++]=c;

	std::cout << "public key (base58.decode): " << bytesToString(_decode, sizeof(_decode)) << std::endl;

	std::cout << "BRC Address: BRC" << base58 << std::endl;
*/

	return 0 ;
#endif

}


