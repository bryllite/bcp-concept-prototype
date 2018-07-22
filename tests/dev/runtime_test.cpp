#include <bryllite-core.hpp>

using Transaction = CTransaction;
using Transactions = CTransactions;


uint256 random_hash( void )
{
	time_t time = bryllite::timestamp();
	return bryllite::sha256( bryllite::sha256s((byte*)&time, sizeof(time) ) );
}


CBlockHeader random_header( void )
{
	CBlockHeader header( rand() % 1000000, random_hash() );
	header.hashMerkleRoot( random_hash() );
	header.transactions( rand() % 100 );

	return header;
}

Transaction random_transaction(void)
{
	CKeyPair sender, receiver, witness;
	uint64_t value = (rand() % 100) * 100000000ULL;

	sender.newKeyPair();
	receiver.newKeyPair();
	witness.newKeyPair();

	Transaction tx( sender.address(), receiver.address(), value );
	tx.owner_sign(sender);
	tx.witness_sign(witness);

	return tx;
}

CBlock random_block(size_t idx)
{
	CBlock block(idx, random_hash());

//	block.append_transaction(random_transaction());

	return block;
}


// file header
struct file_header
{
	size_t block_idx;
	size_t block_len;

	file_header() : block_idx( 0 ), block_len( 0 )
	{
	};

	file_header( size_t idx, size_t len ) : block_idx( idx ), block_len( len )
	{
	};
};

// key collision test
int main( int argc, char** argv )
{
	// check for memory leak
	#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	bryllite::clog << __TIMECODE__ << "Hello, Bryllite! " << bryllite::version() << __SRC__ << bryllite::endl;

	log("sizeof(CBlockHeader): %d", sizeof(CBlockHeader));
	log("sizeof(CBlock): %d", sizeof(CBlock));
	log("sizeof(CTransaction): %d", sizeof(CTransaction));
	log("sizeof(CSecret): %d", sizeof(CSecret));
	log("sizeof(CAddress): %d", sizeof(CAddress));

	for (size_t idx = 0; idx < halving_block * 12; idx++)
	{
		uint64_t block_reward = block_reward_for(idx);

		log("block[%d].reward=%lld bryl, %s BRC", idx, block_reward, toBRC(block_reward).c_str());
	}

	int test_count = 100;
	while (--test_count >= 0)
	{
		Transactions txs;
		size_t tx_count = (rand() % 50) + 1;
		for (size_t i = 0; i < tx_count; i++)
		{
			Transaction tx = random_transaction();
			bryllite::clog << "tx[" << i << "]: " << Color::cyan << tx.txid().GetHex() << Color::default << ", tx=" << tx.to_string() << bryllite::endl;

			if (!txs.append(tx))
				wlog("txs.append(): failed");
		}

		uint256 merkle_root_hash = txs.compute_merkle_root();
		bryllite::clog << "merkle_root_hash=" << Color::cyan << merkle_root_hash.GetHex() << bryllite::endl;

		byte stream_buffer[(sizeof(Transaction) + 8) * 102];
		memset(stream_buffer, 0, sizeof(stream_buffer));
		size_t bytes_write = txs.serialize(stream_buffer, sizeof(stream_buffer));
		bryllite::clog << "bytes_write=" << bytes_write << bryllite::endl;

		Transactions txs2;
		size_t bytes_read = txs2.unserialize(stream_buffer, sizeof(stream_buffer));
		bryllite::clog << "bytes_read=" << bytes_read << bryllite::endl;

		uint256 merkle_root_hash2 = txs2.compute_merkle_root();
		bryllite::clog << "merkle_root_hash2=" << Color::cyan << merkle_root_hash2.GetHex() << bryllite::endl;

		if (merkle_root_hash != merkle_root_hash2)
		{
			elog("merkle_root_hash != merkle_root_hash2 : %s %s", merkle_root_hash.GetHex().c_str(), merkle_root_hash2.GetHex().c_str() );
			break;
		}

		Transactions txs3;
		tx_count = (rand() % 50) + 1;
		for (size_t i = 0; i < tx_count; i++)
			txs3.append(random_transaction());

		size_t txs3_size = txs3.size();

		if (!txs3.append(txs) || txs3.size() != txs3_size + txs.size() )
		{
			elog("txs3.append() failed");
			break;
		}
		uint256 merkle_root_hash3 = txs3.compute_merkle_root();

		bryllite::clog << "txs.size()=" << txs.size() << ", merkle=" << merkle_root_hash.GetHex() << bryllite::endl;
		bryllite::clog << "txs3.size()=" << txs3.size() << ", merkle=" << merkle_root_hash3.GetHex() << bryllite::endl;
	}

	log("test completed");

	return 0;
}


