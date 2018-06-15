/*
@file: message.inl
@desc: message inline header
@author: jade@bryllite.com
@date: 2018-05-20
@note: packet message
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_MESSAGE_INL_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_MESSAGE_INL_

#include "uint256.hpp"
#include "block.hpp"
#include "key.hpp"


// default service port
enum {
	node_default_port = 8300,			// node <-> node default port
	node_bridge_default_port = 8400,	// node <-> bridge server default port
	bridge_game_default_port = 8500,	// bridge server <-> game server default port
	game_default_port = 8600,			// game server <-> game client default port
	node_game_default_port = 8700,		// game client <-> node default port
};

enum {
	max_message_length = (64*1024)-8,	// max packet size 64k - 8 ( for udp header 8 )
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
	message_travel_id,	// travel packet for test: node -> bridge server -> game server -> game client -> node
	message_signed_travel_id,

	message_common_end,
	// common message end

	////////////////////////////////
	// node <-> node message start
	node_message_start = 1000,

	node_message_end,
	// node <-> node message end

	////////////////////////////////////////
	// node <-> bridge server message start
	node_bridge_message_start = 2000,
	node_bridge_message_pop_id,
	node_bridge_message_end,
	// node <-> bridge server message end

	////////////////////////////////////////////////
	// bridge server <-> game server message start
	bridge_game_message_start = 3000,
	bridge_game_message_pop_id,
	bridge_game_message_end,
	// bridge server <-> game server message end

	/////////////////////////////////////////////
	// game server <-> game client message start
	game_message_start = 4000,
	game_message_pop_id,
	game_message_end,
	// game server <-> game client message end

	///////////////////////////////////////
	// node <-> game client message start
	node_client_message_start = 5000,
	node_client_message_pop_id,
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

// sign information
class signing
{
public:
	uint264 _public_key;
	uint512 _signature;

public:
	signing() {
	};

	signing( uint264 public_key, uint512 sig ) : _public_key( public_key ), _signature( sig ) {
	};

	bool verify_for( uint256 hash ) {
		return bryllite::do_verify( _public_key, hash, _signature );
	};
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

// message travel for test
class message_travel : public message
{
public:
	size_t _id;

public:
	message_travel( size_t id ) : _ctor( message_travel ), _id( id ) {
	};
};

// message travel sign
class message_signed_travel : public message
{
public:
	message_travel _travel;
	uint264 _public_key;
	uint512 _signature;

public:
	message_signed_travel( size_t id ) : _ctor( message_signed_travel ), _travel(id) {
	};
};

// node server -> bridge server: request block header relay to bridge server
class node_bridge_message_pop : public message
{
	using block_header = bryllite::block_header;
public:
	block_header _header;

public:
	node_bridge_message_pop( block_header header ) : _ctor( node_bridge_message_pop ), _header(header) {
	};
};


// bridge server -> game server : request block header relay to game server
class bridge_game_message_pop : public message
{
	using block_header = bryllite::block_header;
	using signing = bryllite::net::signing;
public:
	block_header _header;
//	signing _bridge_sign;

public:
//	bridge_game_message_pop( block_header header, signing bridge_sign) : _ctor( bridge_game_message_pop ), _header(header), _bridge_sign(bridge_sign) {
//	};
	bridge_game_message_pop( block_header header ) : _ctor( bridge_game_message_pop ), _header( header ) {
	};

};

// game server -> game client : sending block header to game client
class game_message_pop : public message
{
	using block_header = bryllite::block_header;
	using signing = bryllite::net::signing;
public:
	block_header _header;
//	signing _bridge_sign;
//	signing _game_sign;

public:
//	game_message_pop( block_header header, signing bridge_sign, signing game_sign ) : _ctor( game_message_pop ), _header(header), _bridge_sign(bridge_sign), _game_sign(game_sign) {
//	};
	game_message_pop( block_header header ) : _ctor( game_message_pop ), _header( header ) {
	};
};


// game client -> node server : sending user-signed block header
class node_client_message_pop : public message
{
	using block_header = bryllite::block_header;
	using signing = bryllite::net::signing;
public:
	block_header _header;
//	block_header _user_header;
//	signing _bridge_sign;
//	signing _game_sign;

public:
//	node_client_message_pop( block_header header, block_header user_header, signing bridge_sign, signing game_sign ) 
//		: _ctor( node_client_message_pop ), _header( header ), _user_header(user_header), _bridge_sign( bridge_sign ), _game_sign( game_sign ) {
//	};
	node_client_message_pop( block_header header ) : _ctor( node_client_message_pop ), _header( header ) {
	};
};



#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_MESSAGE_INL_

