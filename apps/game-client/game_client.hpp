#pragma once


#include <tcp_client.hpp>

// game_client 
class game_client : public bryllite::net::tcp_client
{
	using timer = bryllite::timer;
	using udp = boost::asio::ip::udp;

protected:
	// node_server udp client ( send only )
	boost::asio::io_context& _ioc;
	udp::socket _to_node_socket;
	udp::endpoint _node_endpoint;
	boost::asio::streambuf _node_write_buffer;

	bryllite::key_pair _key;	// client private/public key

public:
	game_client( boost::asio::io_context& ioc ) : _ioc(ioc), _to_node_socket(ioc) {
	};

	bool start_client( const char* game_host, unsigned short game_port, const char* node_host, unsigned short node_port );
	bool stop_client( void );

	void send_to_node( message* msg );
	void send_to_node( const char* data, size_t size );

protected:
	void onConnected( bool connected );
	void onDisconnected( void );
	void onWrited( size_t bytes_transferred );
	void onReceived( message* msg );
	void onPing( message_ping* msg );
	void onPong( message_pong* msg );
	void onTravel( message_travel* msg );
	void onPop( game_message_pop* msg );

};
