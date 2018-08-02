#pragma once


typedef int BlockIdx;
enum { INVALID_BLOCK_INDEX = -1 };

// block header class
class CBlockHeader
{
public:
	enum HashType
	{
		Unsigned,
		NodeSigned,
		UserSigned
	};

protected:
	BlockIdx _idx;				// block index
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
	CBlockHeader( BlockIdx blockIdx, uint256 hashPrevBlock );

	// blockIndex, version, timestamp
	BlockIdx idx( void ) const ;
	uint32_t version( void ) const ;
	time_t timestamp( void ) const ;
	void setNull(void);
	bool isNull( void ) const;
	explicit operator bool() const;

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
	bool node_secret_owner(const CAddress& address) const;

	// user sign & verify
	bool user_sign( CKeyPair keyPair );
	bool user_sign( CSecret sign );
	bool user_verify( void ) const ;
	CSecret& user_secret(void);
	bool user_secret_owner(const CAddress& address) const;

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

