#pragma once

/*
// transaction class
class CTransaction
{
protected:
	time_t _timestamp;
	CAddress _from;
	CAddress _to;
	uint64_t _value;
	bool _coinbase;

	CSign _sign;

public:
	CTransaction();
	CTransaction( std::string from, std::string to, uint64_t value );
	CTransaction( std::string to, uint64_t value );

	time_t timestamp( void ) const;
	std::string from( void ) const;
	std::string to( void ) const;
	uint64_t value( void ) const;
	bool coinbase(void) const;

	// txid
	uint256 txid( void ) const;

	// sign
	bool sign( uint264 publicKey, uint256 privateKey );
	bool sign( CKeyPair keyPair );

	// verify
	bool verify( void ) const;

	// serialize/unserialize
	size_t serialize( byte* buffer, size_t buffer_len );
	size_t unserialize( byte* buffer, size_t buffer_len );

	// to string
	std::string to_string( void ) const ;
};

using CTransactions = std::vector< CTransaction >;
*/

/*
// transaction lists
class CTransactions
{
protected:
	std::map< uint256, CTransaction > _transactions;

public:
	CTransactions();

	size_t size( void );

	bool add_transaction( CTransaction tx );
	size_t transactions( std::vector< CTransaction >& txs );
	bool remove_transaction( CTransaction tx );
	
	bool serialize( byte* buffer, size_t buffer_len );
	bool unserialize( byte* buffer, size_t buffer_len );
};
*/



/*
// tx input
class CTxInput
{
protected:
	address _address;
	uint64_t _value;

public:
	CTxInput();
	CTxInput( address addr, uint64_t val );

	address address(void);
	uint64_t value(void);

	uint256 hash(void);
	std::string hash_string(void);
};

// tx output
class CTxOutput
{
protected:
	address _address;
	uint64_t _value;
	bool _coinbase;

public:
	CTxOutput();
	CTxOutput( address addr, uint64_t val );

	address address(void);
	uint64_t value(void);

	uint256 hash(void);
	std::string hash_string(void);
};

// tx class
class CTransaction
{
protected:
	time_t _timestamp;
	CTxInput _input;
	CTxOutput _output;

public:
	CTransaction();

};
*/


class CTransaction
{
protected:
	// tx type ( normal, coinbase, contract, ... )
	enum class Type
	{
		NullTransaction,	// null transaction
		NormalTransaction,	// normal transaction ( transfer )
		CoinBase,			// coinbase block reward transaction ( block reward, event block reward )
		Contract,			// Contract transaction
	};

	enum class HashType
	{
		Unsigned,			// data hash
		OwnerSigned,		// include owner secret
		WitnessSigned,		// include witness secret
	};

protected:
	Type _type;						// tx type
	time_t _timestamp;				// timestamp
	uint64_t _value;				// value ( 1 BRC = 100000000 )
	uint64_t _fee;					// fee
	CAddress _receiver;				// receive address
	CAddress _sender;				// sender address
	CSecret _owner_secret;			// owner secret sig
	CSecret _witness_secret;		// witness secret sig

public:
	// ctor
	CTransaction();
	CTransaction(std::string sender, std::string receiver, uint64_t value );
	CTransaction(std::string receiver, uint64_t value );

	// is null tx?
	void setNull(void);
	bool isNull(void) const;
	explicit operator bool() const;

	// accessor
	Type type(void) const;
	time_t timestamp(void) const;
	uint64_t value(void) const;
	uint64_t fee(void) const;
	std::string receiver(void) const;
	std::string sender(void) const;
	const CSecret& owner_secret(void) const;
	const CSecret& witness_secret(void) const;

	// txid
	uint256 txid(void) const ;

	// is coinbase?
	bool coinbase(void) const;

	// owner sign & verify
	bool owner_sign(CKeyPair keyPair);
	bool owner_verify(void) const;

	// witness sign & verify
	bool witness_sign(CKeyPair keyPair);
	bool witness_verify(void) const;

	// hash transaction
	uint256 hash(HashType hashType) const;

	// to string
	std::string to_string(void) const;
};

// null transaction
extern CTransaction NullTransaction;


// transactions class
class CTransactions 
{
public:
	enum { max_transactions_size = 60 * 1024 };
	enum { max_transaction_count = 200 };
protected:
	// lock object
//	bryllite::lockable _lock;

	// transaction map
	std::map< uint256, CTransaction > _tx_map;

	// transactions
	std::vector< CTransaction > _transactions;

public:
	// ctor
	CTransactions();
	CTransactions(const CTransaction& tx);
	CTransactions(const CTransactions& txs);

	// txs size
	size_t size(void) const;

	// clear all
	void clear(void);

	// append transaction(s)
	bool append(const CTransaction& tx);
	size_t append(const CTransactions& txs);

	// remove transaction
	bool remove(uint256 txid);
	bool remove(const CTransaction& tx);
	size_t remove(const CTransactions& txs);

	// find transaction
	bool find(uint256 txid, CTransaction& tx);
	bool get(size_t idx, CTransaction& tx);

	// create merkle root hash
	uint256 compute_merkle_root(void) const;

	// verify all tx
	bool verify(void);

	// serialize/unserialize
	size_t serialize(byte* stream, size_t stream_len);
	size_t unserialize(byte* stream, size_t stream_len);

	operator std::vector<CTransaction>&(void);

	// to string
	std::string to_string(void) const;
};
