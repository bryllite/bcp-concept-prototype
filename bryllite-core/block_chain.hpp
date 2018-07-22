#pragma once


// block chain class
class CBlockChain 
{
public:
	bryllite::lockable& _lock;
	std::map< size_t, CBlock > _mapBlocks;
	std::map< std::string, uint64_t > _accountBalances;

	// thread for file write
	std::unique_ptr< std::thread > _io_thread;
	std::string _export_file_name;

	bool _stop;

	size_t _last_block_index;

	std::string& _data_dir;

	struct file_header
	{
		char signature[4];
		size_t block_size;

		file_header() : file_header(0)
		{
		};

		file_header( size_t size ) : signature("BRC"), block_size(size)
		{
		};

		bool valid( void )
		{
			return ( signature[0] == 'B' && signature[1] == 'R' && signature[2] == 'C' && signature[3] == 0 );
		};
	};

	// block header
	struct block_header
	{
		size_t block_idx;
		size_t block_len;

		block_header() : block_idx(0), block_len(0)
		{
		};

		block_header( size_t idx, size_t len ) : block_idx(idx), block_len(len)
		{
		};
	};

public:
	CBlockChain(std::string& data_dir, bryllite::lockable& lock);

	bool start(std::string file);
	void stop(void);

	bool loadDB( std::string fileName );
	bool saveDB( std::string fileName );

	// block height & last block index
	size_t size( void );
	size_t last_block_index( void );

	// verify block
	bool verify_block( CBlock block_ );

	// append block
	bool append_block( CBlock block_ );

	// update balance
	bool update_balance( CBlock block_ );

	CBlock find_block( size_t block_idx );
	bool find_block( size_t block_idx, CBlock& block );

	// get balance of account
	uint64_t balanceOf( std::string account );

	// get block header hash
	uint256 get_block_hash( size_t block_idx );

	// block exists?
	bool block_exists( size_t block_idx );

	// block synchronized?
	bool synchronized_for( size_t block_idx );

	// block chain dump
	std::string to_string( void );

protected:
	bool _saveDB(std::string fileName);
	void io_worker(void);
};
