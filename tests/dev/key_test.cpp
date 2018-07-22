#include <cstdio>
#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"

#include <openssl/ripemd.h>
#include "base58.hpp"

/*
#include "bcp_server.hpp"


class node_server : public iface_bcp_server
{
protected:
	bcp_server _bcp_server;

public:
	node_server() : _bcp_server(*this)
	{
	};

	bool start_server( void )
	{
		return _bcp_server.start();
	};

	bool stop_server( void )
	{
		return _bcp_server.stop();
	};

public:
	int on_bcp_new_round_ready( void )
	{
		return 0;
	};
	int on_bcp_new_round_start_for( size_t block_round )
	{
		return 0;
	};
	int on_bcp_step_propose_start_for( void )
	{
		return 0;
	};
	int on_bcp_step_vote_start_for( void )
	{
		return 0;
	};
	int on_bcp_step_commit_start_for( void )
	{
		return 0;
	};
	int on_bcp_new_block_commit_for( void )
	{
		return 0;
	};
	int on_bcp_consensus_fail( void )
	{
		return 0;
	};

public:
	size_t all_peer_count( void )
	{
		return 0;
	};
};


*/





// key collision test
int main( int argc, char** argv ) 
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	log( "Hello, Bryllite! %s", bryllite::version() ) ;

	time_t t = time(0);
	uint256 hash = bryllite::sha256((byte*)&t, sizeof(t) );
	log( "hash=%s", hash.GetHex().c_str() );

	std::string s_hash = hash.GetHex();
	uint256 h1 = bryllite::sha256(s_hash);
	uint256 h2 = bryllite::sha256((byte*)s_hash.c_str(), s_hash.length() );

	log( "h1=%s, h2=%s", h1.GetHex().c_str(), h2.GetHex().c_str() );
	return true;


	// account
	bryllite::account _test_account;
	_test_account.generateNewAccount();

	// prev block hash
	uint256 prevBlockHash;

	// new block
	bryllite::block new_block( 1, prevBlockHash );

	// coinbase block reward
	uint64_t block_reward = 100;
	bryllite::transaction tx_coinbase( _test_account.address(), block_reward );
	tx_coinbase.sign( _test_account.privateKey(), _test_account.publicKey() );

	// append tx
	if ( !new_block.append_transaction( tx_coinbase ) )
	{
		wlog( "new_block.append_transaction() failed. tx_coinbase=%s", tx_coinbase.to_string().c_str() );
		return 0;
	}

	uint256 header_hash = new_block.get_header_hash();
	wlog( "before sign: header_hash: %s", header_hash.GetHex().c_str() );

	new_block.sign( _test_account.privateKey(), _test_account.publicKey() );
	header_hash = new_block.get_header_hash();
	wlog( "after sign: header_hash: %s", header_hash.GetHex().c_str() );

	if ( !new_block.verify() )
	{
		wlog( "new_block.verify() failed" );
		return 0;
	}


	return 0;
	std::map< size_t, size_t > key_length_map;

	size_t _idx = 0;

	while (++_idx < 1000000)
	{
		bryllite::key_pair test_key;
		test_key.generate_new();

		// address from public_key
		std::string s_address = bryllite::make_address_from_public_key(test_key.get_public_key());
//		log("address=%s, length=%d", s_address.c_str(), s_address.length());
		size_t len = s_address.length();

		std::vector<byte> vch;
		if (!decode_base58_check(s_address, vch))
		{
			log("decode_base58_check() failed");
			break;
		}

		if (key_length_map.count(len) > 0) {
			key_length_map[len]++;
		}
		else {
			key_length_map[len] = 1;
		}

		if (_idx % 100 == 0) 
		{
			log("[key_length_map]:%d", _idx);
			for (auto& pair : key_length_map)
			{
				size_t length = pair.first;
				size_t count = pair.second;
				
				float rate = 100.0f * count / _idx;
				log("length[%d]: %d(%.2f%%)", length, count, rate );
			}
		}
	}

	return 0;

	std::map< std::string, uint264 > keyMap;
	unsigned int idx = 0, collision = 0;
	bryllite::timer ts;
	while( true )
	{
		// create key pair
		bryllite::key_pair _key_pair;
		_key_pair.generate_new();

		// key address
		std::string addr = _key_pair.address();

		// check for collision
		if ( keyMap.count( addr ) > 0 ) 
		{
			std::cout << "[ERROR]!!: key collision! address=" << addr << std::endl;
			collision++ ;
		} else {
			keyMap.insert( std::make_pair( addr, _key_pair.get_public_key() ) );
		}

		// print collision status
		if ( ++idx % 100 == 0 ) {
			std::cout << "[" << idx << "] elapsed time=" << ts.laptime() << "(ms) keyCnt=" << keyMap.size() << ", collision=" << collision << std::endl;
		}
	};

	std::cout << "[" << idx << "] elapsed time=" << ts.laptime() << "(ms) keyCnt=" << keyMap.size() << ", collision=" << collision << std::endl;
	return 0;
}


