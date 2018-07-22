/*
@file: message.inl
@desc: message inline header
@author: jade@bryllite.com
@date: 2018-05-20
@note: packet message
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_MESSAGE_INL_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_MESSAGE_INL_



// default service port
enum {
	node_default_port = 8300,			// node <-> node default port
	node_bridge_default_port = 8400,	// node <-> bridge server default port
	bridge_game_default_port = 8500,	// bridge server <-> game server default port
	game_default_port = 8600,			// game server <-> game client default port
	node_game_default_port = 8700,		// game client <-> node default port
};

enum {
	max_message_length = (64*1024),		// max packet size 64k
//	max_payload_length = max_message_length - 10,	// max payload size 64k - 8 - 10(for sizeof(message_header)
//	max_payload_length = 32*1024,	// max payload size 64k - 8 - 10(for sizeof(message_header)
};



// message protocol
// real message must end with _id
enum message_id {
	message_start = 100,

	////////////////////////
	// common message start
	message_common_start = message_start,

	message_ping_id,	// ping?
	message_pong_id,	// pong!

	message_header_sign_req_id,
	message_header_sign_ack_id,

	message_balance_req_id,
	message_balance_ack_id,

	message_transactions_notify_id,

	message_common_end,
	// common message end

	////////////////////////////////
	// node <-> node message start
	node_message_start = 1000,
	node_message_system_start,
		node_message_peer_id_id,
	node_message_system_end,
	node_message_user_start = node_message_start + 100,
		node_message_block_req_id,
		node_message_block_notify_id,
		node_message_new_round_id,
		node_message_propose_id,
		node_message_vote_id,
		node_message_commit_id,
		node_message_verify_block_id,
	node_message_end,
	// node <-> node message end

	////////////////////////////////////////
	// node <-> bridge server message start
	node_bridge_message_start = 2000,
	node_bridge_message_end,
	// node <-> bridge server message end

	////////////////////////////////////////////////
	// bridge server <-> game server message start
	bridge_game_message_start = 3000,
	bridge_game_message_end,
	// bridge server <-> game server message end

	/////////////////////////////////////////////
	// game server <-> game client message start
	game_message_start = 4000,
	game_message_end,
	// game server <-> game client message end

	///////////////////////////////////////
	// node <-> game client message start
	node_client_message_start = 5000,
	node_client_message_end,
	// node <-> game client message end

	message_end,
};


// namespace bryllite::net
namespace bryllite { namespace net {

// 1 bytes align for packet
#pragma pack(push)
#pragma pack(1)

// message header
class message_header
{
public:
	message_header() : message_header( 0, sizeof( message_header ) ) {
	};

	message_header( unsigned short msgid ) : message_header( msgid, sizeof( message_header ) ) {
	};

	message_header( unsigned short msgid, size_t size ) : _msgid( msgid ), _size( size ), _signature( "BRC" ) {
	};

	unsigned short msgid( void ) { return _msgid; };
	void msgid( unsigned short msgid ) { _msgid = msgid; };

	size_t size( void ) { return _size; };
	void size( size_t size ) { _size = size; };

	size_t header_size( void ) { return sizeof(message_header); };
	size_t payload_size( void ) { return ( _size > sizeof(message_header) ) ? _size - sizeof(message_header) : 0; };

	bool valid( void ) 
	{
		return ( _signature[0] == 'B' && _signature[1] == 'R' && _signature[2] == 'C' && _signature[3] == 0 )
			&& ( _size >= sizeof(message_header) && _size <= max_message_length );
	};

	const char* signature( void ) { return _signature; };

protected:
	char _signature[4];
	size_t _size;
	unsigned short _msgid;
//	unsigned short _ver;
};

#pragma pack(pop)	// bytes align for packet


}};// namespace bryllite::net

// message
class message : public bryllite::net::message_header
{
public:
	message( unsigned short id, size_t size ) : message_header( id, size ) {
	};

	// copy string
	void copy_string( char* target, size_t target_length, const char* source, size_t source_length )
	{
		memset( target, 0, target_length );
		memcpy( target, source, ( source_length<target_length ) ? source_length : target_length );
	};
};

// message ctor macro
#define _ctor(_msg)		message( _msg##_id, sizeof(*this) )

// message ping
class message_ping : public message
{
public:
	size_t _id;

public:
	message_ping( size_t id ) : _ctor( message_ping ), _id( id ) {
	};
};

// message pong
class message_pong : public message
{
public:
	size_t _id;

public:
	message_pong( size_t id ) : _ctor( message_pong ), _id( id ) {
	};
};


// block header sign request 
class message_header_sign_req : public message
{
public:
	CBlockHeader _header;
	CSecret _bridge_server_secret;
	CSecret _game_server_secret;

public:
	message_header_sign_req( CBlockHeader header ) : _ctor( message_header_sign_req ), _header(header) 
	{
	};

	message_header_sign_req( CBlockHeader header, CSecret bridge_server_secret ) 
		: _ctor( message_header_sign_req ), _header(header), _bridge_server_secret(bridge_server_secret)
	{
	};

	message_header_sign_req( CBlockHeader header, CSecret bridge_server_secret, CSecret game_server_secret ) 
		: _ctor( message_header_sign_req ), _header(header), _bridge_server_secret(bridge_server_secret), _game_server_secret(game_server_secret)
	{
	};
};

// block header sign ack
class message_header_sign_ack : public message
{
public:
	CBlockHeader _header;
	CSecret _bridge_server_secret;
	CSecret _game_server_secret;

public:
	message_header_sign_ack( CBlockHeader header ) : _ctor( message_header_sign_ack ), _header(header)
	{
	};

	message_header_sign_ack( CBlockHeader header, CSecret bridge_server_secret ) 
		: _ctor( message_header_sign_ack ), _header(header), _bridge_server_secret(bridge_server_secret)
	{
	};

	message_header_sign_ack( CBlockHeader header, CSecret bridge_server_secret, CSecret game_server_secret ) 
		: _ctor( message_header_sign_ack ), _header(header), _bridge_server_secret(bridge_server_secret), _game_server_secret(game_server_secret)
	{
	};
};

class message_balance_req : public message
{
public:
	char _address[CAddress::max_length + 1];

public:
	message_balance_req( std::string address ) : _ctor( message_balance_req )
	{
		memset( _address, 0, sizeof( _address ) );
		copy_string( _address, sizeof( _address ), address.c_str(), address.length() );
	};

	std::string address( void )
	{
		return std::string( _address );
	};
};

class message_balance_ack : public message
{
public:
	char _address[CAddress::max_length + 1];
	uint64_t _balance;

public:
	message_balance_ack( std::string address, uint64_t balance ) : _ctor( message_balance_ack ), _balance(balance)
	{
		memset( _address, 0, sizeof( _address ) );
		copy_string( _address, sizeof( _address ), address.c_str(), address.length() );
	};

	std::string address( void )
	{
		return std::string( _address );
	};

	uint64_t balance( void )
	{
		return _balance;
	};
};

// transactions notify
class message_transactions_notify : public message
{
public:
	size_t _lifetime;
	byte _transactions_buffer[CTransactions::max_transactions_size + 1];

	message_transactions_notify(size_t lifetime) : _ctor( message_transactions_notify ), _lifetime(lifetime)
	{
		memset(_transactions_buffer, 0, sizeof(_transactions_buffer));
	};

	message_transactions_notify( size_t lifetime, CTransactions txs ) : _ctor( message_transactions_notify ), _lifetime(lifetime)
	{
		txs.serialize(_transactions_buffer, sizeof(_transactions_buffer));
	};

	// get transactions
	bool transactions(CTransactions& txs)
	{
		return txs.unserialize(_transactions_buffer, sizeof(_transactions_buffer))>0;
	};
};


// peer id
class node_message_peer_id : public message
{
public:
	PeerData _peer_data;
	CSecret _secret;
	size_t _block_height;

public:
	node_message_peer_id( PeerData peer_data, CSecret secret, size_t block_height ) : _ctor(node_message_peer_id), _peer_data(peer_data), _secret(secret), _block_height(block_height)
	{
	};

	CSecret& secret(void)
	{
		return _secret;
	};
};

class node_message_block_req : public message
{
public:
	size_t _block_idx;

public:
	node_message_block_req( size_t block_idx ) : _ctor(node_message_block_req), _block_idx(block_idx)
	{
	};

	size_t idx( void )
	{
		return _block_idx;
	};
};

class node_message_block_notify : public message
{
public:
	size_t _block_idx;
	byte _block_data[ CBlock::max_block_size + 1 ];

public:
	node_message_block_notify( size_t block_idx, CBlock block ) : _ctor(node_message_block_notify), _block_idx(block_idx)
	{
		block.serialize( _block_data, sizeof(_block_data) );
	};

	size_t idx( void )
	{
		return _block_idx;
	};

	CBlock block( void )
	{
		CBlock b;
		b.unserialize( _block_data, sizeof(_block_data) );
		return b;
	}
};


class node_message_new_round : public message
{
public:
	size_t _new_block_index;

public:
	node_message_new_round( size_t new_block_index ) : _ctor( node_message_new_round ), _new_block_index( new_block_index ) {
	};
};

class node_message_propose : public message
{
public:
	CBlockHeader _block_header;

public:
	node_message_propose( CBlockHeader header ) : _ctor( node_message_propose ), _block_header( header ) {
	};
};

class node_message_vote : public message
{
public:
	CBlockHeader _block_header;

public:
	node_message_vote( CBlockHeader header ) : _ctor( node_message_vote ), _block_header( header ) {
	};
};

class node_message_commit : public message
{
public:
	byte _block[ CBlock::max_block_size + 1 ];
	CSecret _secret;

public:
	node_message_commit( CBlock block, CSecret secret ) : _ctor( node_message_commit ) , _secret(secret)
	{
		block.serialize( _block, sizeof( _block ) );
	};

	CBlock block( void )
	{
		CBlock b;
		b.unserialize( _block, sizeof( _block ) );
		return b;
	};

	CSecret& secret( void )
	{
		return _secret;
	};
};

class node_message_verify_block : public message
{
public:
	byte _block[ CBlock::max_block_size + 1 ];

public:
	node_message_verify_block( CBlock b ) : _ctor( node_message_verify_block )
	{
		b.serialize( _block, sizeof(_block) );
	};

	CBlock get( void )
	{
		CBlock b;
		b.unserialize( _block, sizeof(_block) );
		return b;
	}
};




#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_MESSAGE_INL_

