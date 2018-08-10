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
	CBlock( BlockIdx idx, uint256 hashPrevBlock );

	void setNull(void);
	bool isNull( void ) const;
	explicit operator bool() const;

	// add transaction
	bool append_transaction( CTransactions& txs );
	bool append_transaction( CTransaction& tx, bool update_merkle_hash = true );

	size_t transactions( bool container );
	const CTransactions& transactions(void) const;
	bool transaction(size_t idx, CTransaction& tx);
	bool transaction(uint256 txid, CTransaction& tx);

	// block id
	BlockIdx idx( void ) const;

	// header
	const CBlockHeader& header( void ) const;
	void header( const CBlockHeader& hdr );

	// hash
	uint256 hash( HashType type = HashType::UserSigned ) const;

	// prev block hash
	uint256 prev_hash( void );
	std::string prev_hash_string( void );

	bool node_sign( CKeyPair keyPair );
	bool node_sign( CSecret sign );
	bool node_verify(void) const;
	CSecret& node_secret( void );
	bool node_secret_owner(const CAddress& address) const;

	bool user_sign( CKeyPair keyPair );
	bool user_sign( CSecret sign );
	CSecret& user_secret( void );
	bool user_verify(void) const;
	bool user_secret_owner(const CAddress& address) const;

	std::string to_string(void) const;

	size_t serialize( byte* target, size_t target_len );
	size_t unserialize( byte* src, size_t src_len );
	size_t serialize(CByteStream& bstream) const;
	size_t unserialize(CByteStream& bstream);

	bool operator==( const CBlock& other ) const;
	bool operator!=( const CBlock& other ) const;
	bool operator<( const CBlock& other ) const;
	bool operator<=( const CBlock& other ) const;
	bool operator>( const CBlock& other ) const;
	bool operator>=( const CBlock& other ) const;

protected:
	bool update(void);
};

