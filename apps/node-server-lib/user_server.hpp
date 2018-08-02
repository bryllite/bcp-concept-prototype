#pragma once


// user server callback interface
class IUserServer
{
public:
	virtual int onUserAccept( CTcpSession* session ) = 0;
	virtual int onUserDisconnected( CTcpSession* session, int reason ) = 0;
	virtual int onUserWrite( CTcpSession* session, message* msg, size_t bytes_transferred ) = 0;
	virtual int onUserMessage( CTcpSession* session, message* msg ) = 0;
};

// user server class
class CUserServer : ITcpServer
{
protected:
	IUserServer& _user_server_callback;
	CTcpServer _tcp_server;

public:
	CUserServer(boost::asio::io_service& ios, IUserServer& user_server_callback);

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
