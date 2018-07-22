#pragma once

#include <message.inl>

enum 
{
	node_message_start_id = node_message_start,

	node_message_peer_notify_id,

	node_message_end_id,
};

class node_message_peer_notify : public message
{
public:
	enum { max_peer_id_length = 32 };
	enum { max_host_length = 16 };

	char _node_id[ max_peer_id_length ];
	char _host[ max_host_length ];
	unsigned short _port;

public:
	node_message_peer_notify( const char* node_id, const char* host, unsigned short port )
		: _ctor( node_message_peer_notify ), _port(port)
	{
		copy_string( _node_id, sizeof(_node_id), node_id, strlen(node_id) );
		copy_string( _host, sizeof(_host), host, strlen(host) );
	};

};

