#pragma once

#include "message.inl"

// node message
enum node_message
{
	node_message_start_id = node_message_user_start,

	// request/answer current block chain height
	node_message_block_height_req_id,
	node_message_block_height_ack_id,

	// request/answer block data
	node_message_block_req_id,
	node_message_block_ack_id,

	// bcp consensus protocol control command
	node_message_new_round_id,
	node_message_propose_id,
	node_message_vote_id,
	node_message_commit_id,
	node_message_block_transfer_req_id,
	node_message_block_transfer_ack_id,
	node_message_verify_block_id,

	node_message_end_id = node_message_end
};


class node_message_block_height_req : public message
{
public:
	size_t _block_height;

public:
	node_message_block_height_req( size_t block_height ) : _ctor( node_message_block_height_req ), _block_height( block_height ) {
	};
};

class node_message_block_height_ack : public message
{
public:
	size_t _block_height;

public:
	node_message_block_height_ack( size_t block_height ) : _ctor( node_message_block_height_ack ), _block_height( block_height ) {
	};
};

class node_message_block_req : public message
{
public:
	size_t _block_index;

public:
	node_message_block_req( size_t block_index ) : _ctor( node_message_block_req ), _block_index( block_index ) {
	};
};

class node_message_block_ack : public message
{
	enum { max_block_size = 16 * 1024 };
	using block = bryllite::block;
public:
	byte _block[max_block_size];

public:
	node_message_block_ack( block b ) : _ctor( node_message_block_ack ) {
		b.serialize( _block, sizeof( _block ) );
	};

	block get( void )
	{
		block b;
		b.unserialize( _block, sizeof( _block ) );
		return b;
	};
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
	using block_header = bryllite::block_header;
public:
	block_header _block_header;

public:
	node_message_propose( block_header header ) : _ctor( node_message_propose ), _block_header( header ) {
	};
};

class node_message_vote : public message
{
	using block_header = bryllite::block_header;
public:
	block_header _block_header;

public:
	node_message_vote( block_header header ) : _ctor( node_message_vote ), _block_header( header ) {
	};
};

class node_message_block_transfer_req : public message
{
	using block_header = bryllite::block_header;
public:
	block_header _block_header;

public:
	node_message_block_transfer_req( block_header header ) : _ctor( node_message_block_transfer_req ), _block_header( header ) {
	};
};

class node_message_block_transfer_ack : public message
{
	enum { max_block_size = 32 * 1024 };
	using block = bryllite::block;
public:
	byte _block[max_block_size+1];

public:
	node_message_block_transfer_ack( block b ) : _ctor( node_message_block_transfer_ack ) {
		b.serialize( _block, sizeof( _block ) );
	};

	block get( void )
	{
		block b;
		b.unserialize( _block, sizeof( _block ) );
		return b;
	};
};

class node_message_commit : public message
{
	enum { max_block_size = 32 * 1024 };
	using block = bryllite::block;
public:
	byte _block[max_block_size+1];

public:
	node_message_commit( block b ) : _ctor( node_message_commit ) {
		b.serialize( _block, sizeof( _block ) );
	};

	block get( void )
	{
		block b;
		b.unserialize( _block, sizeof( _block ) );
		return b;
	};
};

class node_message_verify_block : public message
{
	using block = bryllite::block;

public:
	byte _block[ block::max_block_size + 1 ];

public:
	node_message_verify_block( block b ) : _ctor( node_message_verify_block )
	{
		b.serialize( _block, sizeof(_block) );
	};

	block get( void )
	{
		block b;
		b.unserialize( _block, sizeof(_block) );
		return b;
	}
};

