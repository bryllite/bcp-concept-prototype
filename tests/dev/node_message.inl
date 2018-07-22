#pragma once

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
//	node_message_new_round_id,
//	node_message_propose_id,
//	node_message_vote_id,
//	node_message_commit_id,
//	node_message_block_transfer_req_id,
//	node_message_block_transfer_ack_id,
//	node_message_verify_block_id,

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
public:
	byte _block[max_block_size];

public:
	node_message_block_ack( CBlock b ) : _ctor( node_message_block_ack ) {
		b.serialize( _block, sizeof( _block ) );
	};

	CBlock get( void )
	{
		CBlock b;
		b.unserialize( _block, sizeof( _block ) );
		return b;
	};
};

