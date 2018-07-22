#include <cstdio>
#include <iostream>
#include <string>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"


enum node_message_id 
{
	node_message_start_id = node_message_start,

	node_message_new_round_id,
	node_message_propose_id,
	node_message_vote_id,
	node_message_block_req_id,
	node_message_block_ack_id,
	node_message_commit_id,

	node_message_end_id = node_message_end
};

class node_message_new_round : public message
{
public:
	size_t _block_number;

public:
	node_message_new_round(size_t block_number) : _ctor(node_message_new_round), _block_number(block_number) {
	};
};

class node_message_propose : public message
{
	using block_header = bryllite::block_header;
public:
	block_header _block_header;

public:
	node_message_propose( block_header header ) : _ctor(node_message_propose), _block_header(header) {
	};
};

class node_message_vote : public message
{
	using block_header = bryllite::block_header;
public:
	block_header _block_header;

public:
	node_message_vote( block_header header ) : _ctor(node_message_vote), _block_header(header) {
	};
};

class node_message_block_req : public message 
{
public:
	size_t _block_number;
	uint256 _block_hash;

public:
	node_message_block_req( size_t block_number, uint256 block_hash ) : _ctor(node_message_block_req), _block_number(block_number), _block_hash(block_hash) {
	};
};

class node_message_block_ack : public message 
{
	using block = bryllite::block;
public:
	block _block_data;

public:
	node_message_block_ack( block block_data ) : _ctor(node_message_block_ack), _block_data(block_data) {
	};
};


class node_message_commit : public message
{
	using block = bryllite::block;
public:
	block _block;

public:
	node_message_commit( block blk ) : _ctor(node_message_commit), _block(blk) {
	};
};



enum node_state
{
	node_state_null = 0,

	node_state_waiting,
	node_state_new_round,
	node_state_propose,
	node_state_vote,
	node_state_commit,

	node_state_end
};

// current time sec ( 0-29 )
inline time_t get_current_time_sec( void )
{
	return ( bryllite::timestamp()/1000 ) % 30 ;
};


class node_manager;
class node 
{
	using block_header = bryllite::block_header;
	using block = bryllite::block;
	using transaction = bryllite::transaction;

protected:
	node_manager& _node_manager;
	node_state _node_state;
	int _idx;
	
	bryllite::key_pair _key_pair;

	std::vector< block > _block_chain;					// means block chain ( chain of blocks )
	std::map< int, int > _new_round_messages ;			// new round message stored map for each node
	std::map< int, block_header > _propose_messages;	// proposed block header stored map for each node
	std::map< int, block_header > _vote_messages;		// voted block header stored map for each node
	std::map< uint256, int > _vote_messages_cnt;		// voted message cnt for block hash 

	size_t _current_block_index;						// current block index in consensus ( not block height )
	block _block_nominee;								// nominated block ( by myself )
	block_header _voted_header;							// vote winner block header
	block _block_verify;								// vote winner block header to verify
	int _block_commit_cnt;								// commit cnt ( for _block_verify )

	bool _message_sent[ node_state_end ];

public:
	node( node_manager& nm );

	int idx( void ) { return _idx; };

	bool initialize( int idx )
	{
		_idx = idx;

		set_state( node_state_waiting );
		memset( _message_sent, 0, sizeof(_message_sent) );

		// generate key pair ( account, public key, private key )
		_key_pair.generate_new();

		return true;
	};

	void onMain( void )
	{
		if ( _node_state == node_state_waiting )
		{
			if ( !_message_sent[node_state_waiting] && get_current_time_sec() < 1 ) 
			{
				_message_sent[ node_state_waiting ] = true;
				node_message_new_round new_round( _block_chain.size() + 1 );
				sendall( &new_round );
			}
		}
		else if ( _node_state == node_state_new_round )
		{
			if ( get_current_time_sec() >= 5 )
				set_state( node_state_propose );
		}
		else if ( _node_state == node_state_propose )
		{
			if ( get_current_time_sec() >= 10 )
				set_state( node_state_vote );
		}
		else if ( _node_state == node_state_vote )
		{
			if ( get_current_time_sec() >= 20 || get_current_time_sec() <= 1 )
			{
				wlog( "consensus failed! waiting for new round..." );
				set_state( node_state_waiting );
			}
		}
		else if ( _node_state == node_state_commit )
		{
			// timeout
			if ( get_current_time_sec() <= 1 )
			{
				if ( _current_block_index != _block_chain.size() ) {
					wlog( "consensus failed! waiting for new round..." );
				}

				set_state( node_state_waiting );
			}
		}
	};

	void set_state( node_state state ) 
	{
		if ( _node_state == state ) return;

		log( "[%d]: current state = %d, block_size=%d", _idx, state, _block_chain.size() );

		on_state_end( _node_state );
		_node_state = state;
		on_state_begin( _node_state );
	};

	void on_state_begin( node_state state )
	{
		memset( _message_sent, 0, sizeof(_message_sent) );

		if ( _node_state == node_state_waiting )
		{
			log( "waiting for new round start..." );

			_new_round_messages.clear();
			_propose_messages.clear();
			_vote_messages.clear();
		}
		else if ( _node_state == node_state_new_round )
		{
			_current_block_index = _block_chain.size() + 1;
			_block_nominee = create_block_for( _current_block_index );
			_block_nominee.sign( _key_pair.get_private_key(), _key_pair.get_public_key() );

			log( "block %d nominee: %s ", _current_block_index, _block_nominee.get_header_hash().GetHex().c_str() );
//			std::cout << _block_nominee.dump() << std::endl;
		}
		else if ( _node_state == node_state_propose )
		{
			if ( !_message_sent[ node_state_propose ] ) {
				_message_sent[ node_state_propose ] = true;

				node_message_propose propose( _block_nominee._header );
				sendall( &propose );
			}
		}
		else if ( _node_state == node_state_vote )
		{
			block_header header = find_lowest_proposed();
			node_message_vote vote( header );
			sendall( &vote );
		}
		else if ( _node_state == node_state_commit )
		{
			_block_commit_cnt = 0;

			node_message_block_req req( _current_block_index, _voted_header.get_header_hash() );
			sendall( &req );
		}
	};

	void on_state_end( node_state state )
	{
		if( state == node_state_new_round ) {
		}
		else if ( state == node_state_propose ) {
		}
		else if ( state == node_state_vote ) {
		}
		else if ( state == node_state_commit ) {
		}
	};

	block get_block( size_t block_number )
	{
		for( auto& b : _block_chain )
		{
			if ( b.blockNumber() == block_number )
				return b;
		}

		return block();
	};

	uint256 get_block_hash( size_t block_number )
	{
		uint256 hashPrevBlock;

		if( block_number <= 0 ) return hashPrevBlock;

		return get_block( block_number ).get_header_hash();
	};

	block create_block_for( size_t block_height )
	{
		uint256 hashPrevBlock = get_block_hash( block_height - 1 );
		bryllite::block new_block( block_height, hashPrevBlock );

		// block reward 
		size_t block_reward = 100;
		transaction tx_reward( _key_pair.address(), _key_pair.address(), block_reward );
		tx_reward.sign( _key_pair.get_private_key(), _key_pair.get_public_key() );

		// append block reward tx
		new_block.append_transaction( tx_reward );

		// append memory pooled tx here...
		// random transaction
		int tx_cnt = rand() % 10;
		for( int i = 0; i<tx_cnt; i++ )
		{
			bryllite::key_pair from, to;
			from.generate_new();
			to.generate_new();

			transaction tx( from.address(), to.address(), rand() % 1000 );
			tx.sign( from.get_private_key(), from.get_public_key() );
			new_block.append_transaction( tx );
		}

		return new_block;
	};

	// find lowest block header hash on proposed list
	block_header find_lowest_proposed( void )
	{
		int lowest_idx = -1;
		std::string lowest_hash;

		for( auto& pair : _propose_messages )
		{
			int idx = pair.first;
			block_header& header = pair.second;
			std::string hash = header.get_header_hash().GetHex();

			if ( lowest_idx == -1 || lowest_hash.compare( hash ) > 0 ) 
			{
				lowest_idx = idx;
				lowest_hash = hash;
			}
		}

		if ( lowest_idx >= 0 ) return _propose_messages[lowest_idx];
		return block_header();
	};

	// find +1/2 voted block_header
	bool find_major_voted( block_header& header );

	void sendto( node* n, message* msg );
	void sendall( message* msg );

	int onMessage( node* n, message* msg ) 
	{
		switch( msg->msgid() ) 
		{
		case node_message_new_round_id: return onMessageNewRound(n, (node_message_new_round*)msg );
		case node_message_propose_id: return onMessagePropose( n, ( node_message_propose* )msg );
		case node_message_vote_id: return onMessageVote( n, ( node_message_vote* )msg );
		case node_message_block_req_id: return onMessageBlockReq( n, (node_message_block_req* )msg );
		case node_message_block_ack_id: return onMessageBlockAck( n, (node_message_block_ack* )msg );
		case node_message_commit_id: return onMessageCommit( n, ( node_message_commit* )msg );
		default:
			break;
		}

		return 0;
	};

	int onMessageNewRound( node* n, node_message_new_round* msg );
	int onMessagePropose( node* n, node_message_propose* msg );
	int onMessageVote( node* n, node_message_vote* msg );
	int onMessageBlockReq( node* n, node_message_block_req* msg );
	int onMessageBlockAck( node* n, node_message_block_ack* msg );
	int onMessageCommit( node* n, node_message_commit* msg );
};

class node_manager
{
protected:
	std::vector< node* > _nodes;

public:
	node_manager() {
	};

	bool append_node( node* n ) 
	{
		if ( exists(n) ) return false;
		_nodes.push_back( n );
		return true;
	};

	bool exists( node* n ) 
	{
		for( auto nd : _nodes ) {
			if ( nd == n ) return true;
		}

		return false;
	};

	void sendto( node* n, message* msg )
	{
		n->onMessage( n, msg );
	};

	void sendall( message* msg, node* sender )
	{
		for( auto node : _nodes ) 
		{
			node->onMessage( sender, msg );
		}
	};

	size_t get_peer_count( void ) {
		return _nodes.size();
	};

	void run( void )
	{
		time_t current_sec = get_current_time_sec();
//		log( "current_sec: %d sec", current_sec );

		while( true ) 
		{
			bryllite::sleep(0);

			time_t sec = get_current_time_sec();
			if ( sec != current_sec )
			{
				current_sec = sec ;
//				log( "current_sec: %d sec", current_sec ) ;
			}

			for( auto n : _nodes )
				n->onMain();
		}
	};
};


node::node( node_manager& nm ) : _idx( -1 ), _node_manager( nm ), _node_state( node_state_null ), _current_block_index(-1) 
{
	_node_manager.append_node( this );
};

void node::sendto( node* n, message* msg ) {
	if( n != this ) {
		_node_manager.sendto( n, msg );
	}
};

void node::sendall( message* msg ) {
	_node_manager.sendall( msg, this );
};

// find +1/2 voted block_header
bool node::find_major_voted( block_header& header )
{
	for( auto& pair : _vote_messages_cnt )
	{
		uint256 hash = pair.first;
		size_t vote_cnt = pair.second;

		if( vote_cnt * 2 > _node_manager.get_peer_count() )
		{
			for( auto& p : _vote_messages )
			{
				int idx = p.first;
				block_header& h = p.second;

				if( hash == h.get_header_hash() )
				{
					header = h;
					return true;
				}
			}
		}
	}

	return false;
};

int node::onMessageNewRound( node* n, node_message_new_round* msg )
{
	if( _new_round_messages.count( n->idx() ) > 0 ) return 0;

	_new_round_messages.insert( std::make_pair( n->idx(), msg->_block_number ) );

	// new round message +1/2
	if( _new_round_messages.size() * 2 > _node_manager.get_peer_count() ) {
		set_state( node_state_new_round );
	}

	return 0;
};

int node::onMessagePropose( node* n, node_message_propose* msg )
{
	block_header header( msg->_block_header );
	if ( header.valid() && _propose_messages.count( n->idx() ) == 0 ) 
	{
		_propose_messages.insert( std::make_pair( n->idx(), header ) );
	} 
	else 
	{
		wlog( "invalid block_header: %s", header.get_header_hash().GetHex().c_str() );
	}

	log( "onMessagePropose(): proposed.size()=%d", _propose_messages.size() ) ;
	log( "proposed header: %s", header.get_header_hash().GetHex().c_str() ) ;

	return 0;
};

int node::onMessageVote( node* n, node_message_vote* msg )
{
	block_header header( msg->_block_header );
	if ( header.valid() && _vote_messages.count( n->idx() ) == 0 ) 
	{
		_vote_messages.insert( std::make_pair( n->idx(), header ) );
		
		uint256 hash = header.get_header_hash();
		if ( _vote_messages_cnt.count( hash ) == 0 ) {
			_vote_messages_cnt.insert( std::make_pair( hash, 1 ) );
		} else {
			_vote_messages_cnt[hash]++;
		}
	} 
	else
	{
		wlog( "invalid block_header: %s", header.get_header_hash().GetHex().c_str() );
	}

	log( "onMessageVote(): voted.size()=%d", _vote_messages.size() );
	log( "voted header: %s", header.get_header_hash().GetHex().c_str() );

	// find out +1/2 voted block header
	if ( find_major_voted( _voted_header ) )
	{
		set_state( node_state_commit );
	}

	return 0;
};

int node::onMessageBlockReq( node* n, node_message_block_req* msg )
{
//	if ( n == this ) return 0;
	if ( msg->_block_number != _current_block_index ) {
		wlog( "invalid block_index! req_block_number=%d, current_block_index=%d", msg->_block_number, _current_block_index );
		return 0;
	}

	if ( msg->_block_hash != _block_nominee.get_header_hash() ) {
		return 0;
	}

	node_message_block_ack ack( _block_nominee );
	sendto( n, &ack );

	return 0;
};

int node::onMessageBlockAck( node* n, node_message_block_ack* msg )
{
	_block_verify = msg->_block_data;

	if ( _block_verify.valid() ) {
		node_message_commit commit( _block_verify );
		sendall( &commit );
	}
	else {
		wlog( "onMessageBlockAck() _block_verify.valid() failed" );
	}

	return 0;
};


int node::onMessageCommit( node* n, node_message_commit* msg )
{
	block& b = msg->_block;

	if ( b.valid() && _block_verify.get_header_hash() == b.get_header_hash() ) 
	{
		_block_commit_cnt++ ;

		if ( _block_commit_cnt * 2 > _node_manager.get_peer_count() )
		{
			_block_chain.push_back( b );

			dlog( "new block commited! block=%s, block_size=%d", _block_verify.get_header_hash().GetHex().c_str(), _block_chain.size() );
//			set_state( node_state_waiting );
		}
	} else {
		wlog( "onMessageCommit(): b.valid=%d", b.valid() );
	}


	return 0;
};

// timer test
int main( int argc, char** argv ) 
{
	// check for memory leak
#ifdef _WIN32
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	log( "Hello, Bryllite! %s", bryllite::version() ) ;

	node_manager _node_manager;

	const int node_cnt = 3;
	node* _nodes[node_cnt];
	for( int i=0; i<node_cnt; i++ )
	{
		_nodes[i] = new node(_node_manager);
		_nodes[i]->initialize(i);
	}

	_node_manager.run();

	for( int i=0; i<node_cnt; i++ )
		delete _nodes[i];


	return 0 ;
}


