#if !defined( __BYLLITE_PLATFORM_PROTOCOL_INL_HEADER__ )
#define __BYLLITE_PLATFORM_PROTOCOL_INL_HEADER__

// default service port
enum 
{
	default_p2p_node_port = 8300,
	default_node_bridge_port = 8400,
	default_bridge_game_port = 8500,
	default_game_client_port = 8600,
	default_client_node_port = 8700,
};


// message protocol
enum message_ {
	message_start_id = 100,

	// common message start
	message_common_start = message_start_id,

	message_ping_id,	// ping?
	message_pong_id,	// pong!

	// common message end
	message_common_end,

	// node <-> node message start
	node_message_start = 1000,
	// node <-> node message end
	node_message_end,

	// node <-> bridge message start
	node_bridge_message_start = 2000,
	// node <-> bridge message end
	node_bridge_message_end,

	// bridge <-> game message start
	bridge_game_message_start = 3000,
	// bridge <-> game message end
	bridge_game_message_end,

	// game <-> client message start
	game_client_message_start = 4000,
	// game <-> client message end
	game_client_message_end,

	// node <-> client message start
	node_client_message_start = 5000,
	node_client_message_end,

	message_end_id,
};

// node message
class message : public bryllite::net::message_header 
{
public:
	message( message_ id, size_t size ) : message_header( id, size ) {
	};

	// copy string
	void string_copy( char* target, size_t target_length, const char* source, size_t source_length ) 
	{
		memset( target, 0, target_length );
		memcpy( target, source, min( source_length, target_length ) );
	};
};

#define _ctor(_msg)		message( _msg##_id, sizeof(*this) )

// message ping
class message_ping : public message 
{
public:
	size_t _id;

public:
	message_ping( size_t id ) : _ctor( message_ping ), _id(id) {
	};
};

// message pong
class message_pong : public message 
{
public:
	size_t _id;

public:
	message_pong( size_t id ) : _ctor( message_pong ), _id(id) {
	};
};

#undef _ctor

#endif //__BYLLITE_PLATFORM_PROTOCOL_INL_HEADER__
