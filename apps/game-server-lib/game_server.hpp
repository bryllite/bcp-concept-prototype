#pragma once


// game server class
class game_server : public IUserServer, public ITcpClient, public ICallbackTimer
{
	using tcp = boost::asio::ip::tcp;
	using callback_timer = bryllite::callback_timer;

protected:
	app_config& _app_config;

	boost::asio::io_service _io_service;
	CUserServer _user_server;
	CTcpClient _bridge_client;
	CAccount _game_account;

	std::unique_ptr< std::thread > _io_worker_thread;

	callback_timer _callback_timer;
	enum { timer_id_reconnect = 1024 + 1 };

public:
	game_server(app_config& appConfig);

	// start & stop server
	bool start_server( void );
	bool stop_server( void );

	// reconnect to bridge server
	bool reconnect( void );

	// game server main procedure
	int onMain( void );

	// callback timer handler
	void onTimeOut( timer_id id, void* pContext );

public:
	int onUserServerAccept(CTcpSession* session);
	int onUserServerDisconnect(CTcpSession* session, int reason );
	int onUserServerWrite(CTcpSession* session, message* msg, size_t bytes_transferred );
	int onUserServerMessage( CTcpSession* session, message* msg );

public:
	int onTcpClientConnected( CTcpClient* client, bool connected );
	int onTcpClientDisconnected( CTcpClient* client, int reason );
	int onTcpClientWrite( CTcpClient* client, message* msg, size_t bytes_transferred );
	int onTcpClientMessage( CTcpClient* client, message* msg );
	int onBridgeMessageHeaderSignReq( message_header_sign_req* msg );

protected:
};
