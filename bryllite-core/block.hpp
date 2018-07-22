#pragma once


// block class
class CBlock
{
public:
	enum { max_block_size = 62 * 1024 };
	using HashType = CBlockHeader::HashType;

public:
	// block header
	CBlockHeader _header;

	// transactions
	CTransactions _transactions;

	// node committed secrets
	std::vector<CSecret> _node_secrets;

public:
	CBlock();
	CBlock( size_t idx, uint256 hashPrevBlock );

	bool isNull( void ) const;

	// add transaction
	bool append_transaction( CTransactions& txs );
	bool append_transaction( CTransaction& tx, bool update_merkle_hash = true );

	size_t transactions( bool container );
	CTransactions& transactions(void);
	bool transaction(size_t idx, CTransaction& tx);
	bool transaction(uint256 txid, CTransaction& tx);

	// block id
	size_t idx( void );

	// header
	CBlockHeader& header( void );
	void header( CBlockHeader& hdr );

	// hash
	uint256 hash( HashType type = HashType::UserSigned );

	// prev block hash
	uint256 prev_hash( void );
	std::string prev_hash_string( void );

	bool node_sign( CKeyPair keyPair );
	bool node_sign( CSecret sign );
	bool node_verify(void);
	CSecret& node_secret( void );

	bool user_sign( CKeyPair keyPair );
	bool user_sign( CSecret sign );
	CSecret& user_secret( void );
	bool user_verify(void);

	std::string to_string( bool short_hex_code = true );

	size_t serialize( byte* target, size_t target_len );
	size_t unserialize( byte* src, size_t src_len );

	bool operator==( const CBlock& other ) const;
	bool operator!=( const CBlock& other ) const;
	bool operator<( const CBlock& other ) const;
	bool operator<=( const CBlock& other ) const;
	bool operator>( const CBlock& other ) const;
	bool operator>=( const CBlock& other ) const;

protected:
	bool update(void);
};

