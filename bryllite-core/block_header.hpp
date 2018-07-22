#pragma once


// block header class
class CBlockHeader
{
public:
	enum { invalid_block_index = -1 };
	enum HashType
	{
		Unsigned,
		NodeSigned,
		UserSigned
	};

protected:
	size_t _idx;				// block index
	uint32_t _version;			// version
	time_t _timestamp;			// timestamp
	uint256 _hashPrevBlock;		// prev block hash
	uint256 _hashMerkleRoot;	// merkle tree root hash
	size_t _transactions;		// transaction count

	// node secret
	CSecret _node_secret;

	// user signature
	CSecret _user_secret;

public:
	// ctor
	CBlockHeader();
	CBlockHeader( size_t blockIdx, uint256 hashPrevBlock );

	// blockIndex, version, timestamp
	size_t idx( void ) const ;
	uint32_t version( void ) const ;
	time_t timestamp( void ) const ;
	bool isNull( void ) const;

	// prev block hash
	uint256 hashPrevBlock( void ) const ;
	void hashPrevBlock( uint256 hash );

	// merkle tree root hash
	uint256 hashMerkleRoot( void ) const ;
	void hashMerkleRoot( uint256 hash );

	// transaction count
	size_t transactions( void ) const ;
	void transactions( size_t count );
	
	// node sign & verify
	bool node_sign( CKeyPair keyPair );
	bool node_sign( CSecret sign );
	bool node_verify( void ) const;
	CSecret& node_secret(void);

	// user sign & verify
	bool user_sign( CKeyPair keyPair );
	bool user_sign( CSecret sign );
	bool user_verify( void ) const ;
	CSecret& user_secret(void);

	bool operator==(const CBlockHeader& other ) const;
	bool operator!=(const CBlockHeader& other ) const;
	bool operator<(const CBlockHeader& other ) const; 
	bool operator<=( const CBlockHeader& other ) const;
	bool operator>(const CBlockHeader& other ) const; 
	bool operator>=( const CBlockHeader& other ) const;

	// to string
	std::string to_string( void ) const ;

	// CBlockHeader hash
	uint256 hash( HashType type = UserSigned ) const;

	// prev block hash with sign
	uint256 prev_hash( void ) const;
	std::string prev_hash_string( void ) const;

protected:

};

