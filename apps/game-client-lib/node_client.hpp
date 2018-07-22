#pragma once

class INodeClient
{
public:
	virtual int onNodeClientConnect( CTcpClient* client, bool connected ) = 0;
	virtual int onNodeClientDisconnect( CTcpClient* client, int reason ) = 0;
	virtual int onNodeClientWrite( CTcpClient* client, message* msg, size_t bytes_transferred ) = 0;
	virtual int onNodeClientMessage( CTcpClient* client, message* msg ) = 0;
};

// node client
class CNodeClient : public ITcpClient
{
protected:
	boost::asio::io_service& _ios;

	CTcpClient _tcp_client;
	INodeClient& _callback;

public:
	CNodeClient(boost::asio::io_service& ios, INodeClient& callback );

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
