#pragma once

#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

#include "byte_stream.hpp"


// p2p session
class p2p_session : public bryllite::lockable
{
	using tcp = boost::asio::ip::tcp;
	friend class p2p_server;
protected:
	boost::asio::io_service& _ios;
	tcp::socket _socket;
	enum { max_buffer_size = max_message_length };
	std::atomic<bool> _active;
	time_t _closed ;

	session_buffer<32, max_buffer_size> _read_buffer;
	session_buffer<16, max_buffer_size> _write_buffer;


public:
	p2p_session( boost::asio::io_service& ios );
	virtual ~p2p_session();

	bool active( void );
	void active( bool active_ );

	tcp::socket& socket( void );

	void close( void );
	bool expired( time_t timeout );

	bool write( byte* data, size_t len );
	bool write( message* msg );

	byte* read_buffer( void );
	byte* write_buffer( void );

	byte* next_read_buffer( void );
	byte* next_write_buffer( void );

	size_t read_buffer_length( void );
	size_t write_buffer_length( void );

protected:
	void handle_write( const boost::system::error_code& error, size_t bytes_transferred );
};
