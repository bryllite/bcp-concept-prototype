#pragma once

// bridge server
class bridge_server : public IGameServer, public ITcpClient, public ICallbackTimer
{
	using tcp = boost::asio::ip::tcp;
	using callback_timer = bryllite::callback_timer;

protected:
	// io_service
	boost::asio::io_service _io_service;

	// config
	app_config& _app_config;

	// game server
	CGameServer _game_server;

	// node client
	CTcpClient _node_client;

	// key pair for signing
	CAccount _bridge_account;

	// io worker threads
	std::unique_ptr< std::thread > _io_worker_thread;

	tcp::endpoint _node_endpoint;

	// callback timer
	callback_timer _callback_timer;

public:
	bridge_server(app_config& appConfig);

	// start & stop server
	bool start_server( void );
	bool stop_server( void );

	// reconnect to node
	bool reconnect( void );

	// bridge_server main procedure
	int onMain( void );

	// callback timer
	void onTimeOut( timer_id id, void* pContext );
	enum { timer_id_reconnect = 1024 + 1 };

public:
	int onTcpClientConnected( CTcpClient* tcp_client, bool connected );
	int onTcpClientDisconnected( CTcpClient* tcp_client, int reason );
	int onTcpClientWrite( CTcpClient* tcp_client, message* msg, size_t bytes_transferred );
	int onTcpClientMessage( CTcpClient* tcp_client, message* msg );
	int onNodeMessageHeaderSignReq( message_header_sign_req* msg );

public:
	int onGameServerAccept(CTcpSession* session);
	int onGameServerDisconnect(CTcpSession* session, int reason );
	int onGameServerWrite(CTcpSession* session, message* msg, size_t bytes_transferred );
	int onGameServerMessage( CTcpSession* session, message* msg );
};
