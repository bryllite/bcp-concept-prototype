#pragma once

#include <leveldb/db.h>

// block chain class
class CBlockChain 
{
public:
	bryllite::lockable& _lock;
	std::map< BlockIdx, CBlock > _mapBlocks;
	std::map< std::string, uint64_t > _accountBalances;

	// thread for file write
	std::unique_ptr< std::thread > _io_thread;
	std::string _export_file_name;

	// is stopped?
	bool _stop;

	// last block index
	BlockIdx _last_block_index;

	// data-dir path
	std::string& _data_dir;

	// leveldb
	CLevelDB _ldb;

public:
	CBlockChain(std::string& data_dir, bryllite::lockable& lock);

	bool start(std::string file);
	void stop(void);

	bool rebuildFromDB(void);
	bool insertDB(const CBlock& block);

	// block height & last block index
	size_t size( void );
	BlockIdx last_block_index( void );

	// verify block
	bool verify_block( CBlock block );

	// append block
	bool append_block( CBlock block );

	// update balance
	bool update_balance( CBlock block );

	CBlock find_block( BlockIdx idx );
	bool find_block( BlockIdx idx, CBlock& block );

	// get balance of account
	uint64_t balanceOf( std::string account );

	// get block header hash
	uint256 get_block_hash( BlockIdx idx );

	// block exists?
	bool block_exists( BlockIdx idx );

	// block synchronized?
	bool synchronized_for( BlockIdx idx );

	// block chain dump
	std::string to_string( void );

	BlockIdx nextRound(void);

protected:
	// add block without verification
	bool addBlock(const CBlock& block);

	// rebuild balance data
	bool rebuildBalances(BlockIdx last);

};
