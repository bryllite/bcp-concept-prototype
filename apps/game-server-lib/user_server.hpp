#pragma once

// user server callback
class IUserServer 
{
public:
	virtual int onUserServerAccept(CTcpSession* session) = 0;
	virtual int onUserServerDisconnect(CTcpSession* session, int reason ) = 0;
	virtual int onUserServerWrite(CTcpSession* session, message* msg, size_t bytes_transferred ) = 0;
	virtual int onUserServerMessage( CTcpSession* session, message* msg ) = 0;
};

// user server
class CUserServer : public ITcpServer
{
protected:
	IUserServer& _user_server_callback;
	CTcpServer _tcp_server;

public:
	CUserServer(boost::asio::io_service& ios, IUserServer& game_server_callback);

	// start/stop server
	bool start_server( unsigned short port );
	bool stop_server( void );

	// send message
	bool sendto( CTcpSession* session, message* msg );
	size_t sendall( message* msg );

	size_t user_count( void );

public:
	int onTcpServerAccept( CTcpSession* session );
	int onTcpServerDisconnect( CTcpSession* session, int reason );
	int onTcpServerWrite( CTcpSession* session, message* msg, size_t bytes_trasnferred );
	int onTcpServerMessage( CTcpSession* session, message* msg );
};

