#pragma once

// bridge server callback interface
class IBridgeServer
{
public:
	virtual int onBridgeAccept( CTcpSession* session ) = 0;
	virtual int onBridgeDisconnected( CTcpSession* session, int reason ) = 0;
	virtual int onBridgeWrite( CTcpSession* session, message* msg, size_t bytes_transferred ) = 0;
	virtual int onBridgeMessage( CTcpSession* session, message* msg ) = 0;
};

// bridge server class
class CBridgeServer : ITcpServer
{
protected:
	IBridgeServer& _bridge_server_callback;
	CTcpServer _tcp_server;

public:
	CBridgeServer(boost::asio::io_service& ios, IBridgeServer& bridge_server_callback);

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
