#pragma once

// user client callback interface
class IUserClient
{
public:
	virtual int onUserClientConnect( CTcpClient* client, bool connected ) = 0;
	virtual int onUserClientDisconnect( CTcpClient* client, int reason ) = 0;
	virtual int onUserClientWrite( CTcpClient* client, message* msg, size_t bytes_transferred ) = 0;
	virtual int onUserClientMessage( CTcpClient* client, message* msg ) = 0;
};

// user client
class CUserClient : public ITcpClient
{
protected:
	boost::asio::io_service& _ios;

	CTcpClient _tcp_client;
	IUserClient& _callback;

public:
	CUserClient(boost::asio::io_service& ios, IUserClient& callback );

	bool start( std::string host, unsigned short port );
	void stop( void );

	bool connected( void );
	bool reconnect( void );

	// write message
	bool write( message* msg );

public:
	int onTcpClientConnected( CTcpClient* client, bool connected );
	int onTcpClientDisconnected( CTcpClient* client, int reason );
	int onTcpClientWrite( CTcpClient* client, message* msg, size_t bytes_transferred );
	int onTcpClientMessage( CTcpClient* client, message* msg );
};
