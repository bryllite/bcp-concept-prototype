#pragma once

#include <cstdint>
#include <string>

#include <bryllite-common.hpp>

#include <boost/asio.hpp>

#include "transaction.hpp"
#include "sha256.hpp"


// namespace bryllite
namespace bryllite {

// block header
class block_header
{
	struct header
	{
		size_t _blockNumber;
		uint32_t _version;
		uint256 _hashPrevBlock;
		uint256 _hashMerkleRoot;
		time_t _timestamp;
		size_t _transactionCnt;

		header() : _blockNumber(-1), _version(VERSION_NUMBER), _timestamp(0), _transactionCnt(0) {
		};

		header( size_t blockNumber, uint256 hashPrevBlock ) : _blockNumber(blockNumber), _hashPrevBlock(hashPrevBlock), _version(VERSION_NUMBER), _timestamp(0), _transactionCnt(0) {
		};
	};

public:

	// header
	header _header;

	// user signature
	uint264 _userPublicKey;
	uint512 _userSignature;

	block_header() {
	};

	block_header( size_t blockNumber, uint256 hashPrevBlock ) : _header(blockNumber, hashPrevBlock) {
	};

	bool valid(void);

	size_t blockNumber( void ) { return _header._blockNumber; };
	std::string dump( void );

	bool sign( uint256 private_key, uint264 public_key );

	uint256 get_header_hash( void );
	uint256 get_prev_hash( void );

	bool operator==(const block_header& other ) const;
	bool operator!=(const block_header& other ) const;
	bool operator<(const block_header& other ) const; 
	bool operator<=( const block_header& other ) const;
	bool operator>(const block_header& other ) const; 
	bool operator>=( const block_header& other ) const;
};


// block class ( header + transactions )
class block 
{
public:
	block_header _header;
	transactions _transactions;

public:
	block() {
	};

	block( size_t blockNumber, uint256 hashPrevBlock ) : _header( blockNumber, hashPrevBlock ) {
	};

	bool valid( void );

	bool append_transaction( transactions& txs );
	bool append_transaction( transaction& tx, bool update_merkle_hash = true );

	size_t blockNumber( void ) { return _header.blockNumber(); };

	uint256 get_header_hash( void );
	uint256 get_prev_hash( void );

	bool sign( uint256 private_key, uint264 public_key );

	std::string dump( void );

	size_t serialize( byte* target, size_t target_len );
	size_t unserialize( byte* src, size_t src_len );

	bool operator==( const block& other ) const;
	bool operator!=( const block& other ) const;
	bool operator<( const block& other ) const;
	bool operator<=( const block& other ) const;
	bool operator>( const block& other ) const;
	bool operator>=( const block& other ) const;

	uint256 merkle_root_hash( void );

protected:
	bool update_merkle_root_hash( void );
};

};//namespace bryllite
