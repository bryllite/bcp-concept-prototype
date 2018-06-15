#if !defined( __BYLLITE_PLATFORM_NODE_MESSAGE_INL__ )
#define __BYLLITE_PLATFORM_NODE_MESSAGE_INL__


enum node_message_ {
	node_message_start = 5000,

	node_message_ping_id,
	node_message_pong_id,

	node_message_chat_id,

	node_message_end
};

// node message
class node_message : public bryllite::net::message_header 
{
public:
	node_message( node_message_ id, size_t size ) : message_header( id, size ) {
	};

	// copy string
	void string_copy( char* target, size_t target_length, const char* source, size_t source_length ) 
	{
		memset( target, 0, target_length );
		memcpy( target, source, min( source_length, target_length ) );
	};
};

#define _ctor(msgid)		node_message( msgid##_id, sizeof(*this) )

// node message ping
class node_message_ping : public node_message 
{
public:
	size_t _id;

public:
	node_message_ping( size_t id ) : _ctor( node_message_ping ), _id(id) {
	};
};

// node message pong
class node_message_pong : public node_message 
{
public:
	size_t _id;

public:
	node_message_pong( size_t id ) : _ctor( node_message_pong ), _id(id) {
	};
};

class node_message_chat : public node_message
{
	enum { max_msg_len = 1024 };
public:
	char _msg[ max_msg_len + 1 ];

public:
	node_message_chat( std::string msg ) : _ctor( node_message_chat ) {
		string_copy( _msg, sizeof(_msg), msg.c_str(), msg.length() );
	};
};

#undef _ctor

#endif //__BYLLITE_PLATFORM_NODE_MESSAGE_INL__
