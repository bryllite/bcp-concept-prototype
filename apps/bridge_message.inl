#if !defined( __BYLLITE_PLATFORM_BRIDGE_SERVICE_MESSAGE_INL__ )
#define __BYLLITE_PLATFORM_BRIDGE_SERVICE_MESSAGE_INL__


// bridge server <--> game server protocol
// bridge server <--> node server protocol
enum bridge_game_message_
{
	bridge_game_message_start_id = 2000,

	bridge_game_message_ping_id,
	bridge_game_message_pong_id,

	bridge_game_message_test_id,

	bridge_game_message_end_id,
};

// bridge server <--> node server protocol
enum bridge_node_message_
{
	bridge_node_message_start_id = 3000,

	bridge_node_message_ping_id,
	bridge_node_message_pong_id,

	node_bridge_message_test_id,

	bridge_node_message_end_id,
};


// bridge message
class bridge_message : public bryllite::net::message_header 
{
public:
	bridge_message( unsigned short msgid, size_t size ) : bryllite::net::message_header( msgid, size ) {
	};

	// copy string
	void string_copy( char* target, size_t target_length, const char* source, size_t source_length )
	{
		memset( target, 0, target_length );
		memcpy( target, source, min( source_length, target_length ) );
	};
};

#define __ctor(msgid)		bridge_message( msgid##_id, sizeof(*this) )

// ping message
class bridge_game_message_ping : public bridge_message 
{
public:
	bridge_game_message_ping( size_t id ) : __ctor( bridge_game_message_ping ), _id(id)
	{
	};

public:
	size_t _id ;
};

// pong message
class bridge_game_message_pong : public bridge_message
{
public:
	bridge_game_message_pong( size_t id ) : __ctor( bridge_game_message_pong ), _id( id )
	{
	};

public:
	size_t _id;
};

// ping message
class bridge_node_message_ping : public bridge_message
{
public:
	bridge_node_message_ping( size_t id ) : __ctor( bridge_node_message_ping ), _id( id )
	{
	};

public:
	size_t _id;
};

// bridge -> game test message
class bridge_game_message_test : public bridge_message
{
public:
	bridge_game_message_test( size_t id ) : __ctor( bridge_game_message_test ), _id(id)
	{
	};

public:
	size_t _id;
};


// pong message
class bridge_node_message_pong : public bridge_message
{
public:
	bridge_node_message_pong( size_t id ) : __ctor( bridge_node_message_pong ), _id( id )
	{
	};

public:
	size_t _id;
};

// node -> bridge test message
class node_bridge_message_test : public bridge_message
{
public:
	node_bridge_message_test( size_t id ) : __ctor( node_bridge_message_test ), _id(id)
	{
	};

public:
	size_t _id;
};


#undef __ctor

#endif //__BYLLITE_PLATFORM_BRIDGE_SERVICE_MESSAGE_INL__
