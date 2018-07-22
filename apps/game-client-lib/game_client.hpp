#pragma once


// game client class
class game_client : public IUserClient, public INodeClient, public ICallbackTimer
{
	using tcp = boost::asio::ip::tcp;
	using callback_timer = bryllite::callback_timer;

protected:
	boost::asio::io_service& _ios;
	app_config& _app_config;
	size_t _idx;
	CUserClient _user_client;
	CNodeClient _node_client;
	CAccount _user_account;
	callback_timer _callback_timer;
	enum {
		timer_id_reconnect = 1024 + 1,
		timer_id_balance,
	};

public:
	game_client(boost::asio::io_service& ios, app_config& appConfig);

	// start & stop client
	bool start( size_t idx );
	bool stop( void );

	// reconnect to bridge server
	bool reconnect_game( void );
	bool reconnect_node( void );

	// request balance
	bool request_balance( void );

	// game client main procedure
	int onMain( void );

	// callback timer handler
	void onTimeOut( timer_id id, void* pContext );

	size_t idx( void );

public:
	int onUserClientConnect( CTcpClient* client, bool connected );
	int onUserClientDisconnect( CTcpClient* client, int reason );
	int onUserClientWrite( CTcpClient* client, message* msg, size_t bytes_transferred );
	int onUserClientMessage( CTcpClient* client, message* msg );
protected:
	int onGameMessageHeaderSignReq( message_header_sign_req* msg );

public:
	int onNodeClientConnect( CTcpClient* client, bool connected );
	int onNodeClientDisconnect( CTcpClient* client, int reason );
	int onNodeClientWrite( CTcpClient* client, message* msg, size_t bytes_transferred );
	int onNodeClientMessage( CTcpClient* client, message* msg );
protected:
	int onNodeMessagePing( message_ping* msg );
	int onNodeMessageBalanceAck( message_balance_ack* msg );
};
