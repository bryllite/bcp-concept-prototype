#if !defined( __BYLLITE_PLATFORM_GAME_SERVER_CLIENT_GAME_MESSAGE_INL__ )
#define __BYLLITE_PLATFORM_GAME_SERVER_CLIENT_GAME_MESSAGE_INL__


// game server <--> game client protocol sample
enum game_message_
{
	game_message_start_id = 1000,

	game_message_ping_id,		// ping?
	game_message_pong_id,		// pong!

	game_message_end_id,
};


// game message
class game_message : public bryllite::net::message_header 
{
public:
	game_message( unsigned short msgid, size_t size ) : bryllite::net::message_header( msgid, size ) {
	};

	// copy string
	void string_copy( char* target, size_t target_length, const char* source, size_t source_length )
	{
		memset( target, 0, target_length );
		memcpy( target, source, min( source_length, target_length ) );
	};
};

#define _ctor(msgid)		game_message( msgid##_id, sizeof(*this) )

// ping?
class game_message_ping : public game_message 
{
public:
	game_message_ping( size_t id ) : _ctor( game_message_ping ), _id(id)
	{
	};

public:
	size_t _id ;
};

// pong!
class game_message_pong : public game_message 
{
public:
	game_message_pong( size_t id ) : _ctor( game_message_pong ), _id(id)
	{
	};

public:
	size_t _id ;
};


#undef _ctor

#endif //__BYLLITE_PLATFORM_GAME_SERVER_CLIENT_GAME_MESSAGE_INL__
