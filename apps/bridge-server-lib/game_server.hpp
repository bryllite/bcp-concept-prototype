#pragma once

// game server callback
class IGameServer 
{
public:
	virtual int onGameServerAccept(CTcpSession* session) = 0;
	virtual int onGameServerDisconnect(CTcpSession* session, int reason ) = 0;
	virtual int onGameServerWrite(CTcpSession* session, message* msg, size_t bytes_transferred ) = 0;
	virtual int onGameServerMessage( CTcpSession* session, message* msg ) = 0;
};

// game server
class CGameServer : public ITcpServer
{
protected:
	IGameServer& _game_server_callback;
	CTcpServer _tcp_server;

public:
	CGameServer(boost::asio::io_service& ios, IGameServer& game_server_callback);

	// start/stop server
	bool start_server( unsigned short port );
	bool stop_server( void );

	// send message
	bool sendto( CTcpSession* session, message* msg );
	size_t sendall( message* msg );

public:
	int onTcpServerAccept( CTcpSession* session ) override;
	int onTcpServerDisconnect( CTcpSession* session, int reason ) override;
	int onTcpServerWrite( CTcpSession* session, message* msg, size_t bytes_trasnferred ) override;
	int onTcpServerMessage( CTcpSession* session, message* msg ) override;
};

