#include "stdafx.hpp"
#include "node_server.hpp"


CNodeServer::CNodeServer() : _peer_server(_ios, this), _ping_id(0)
{
}

int CNodeServer::node_id( void ) 
{
	return _peer_server.node_id();
};

bool CNodeServer::start_server( PeerData host_data, int thread_count )
{
	_host_data = host_data;

	try
	{
		// NTP time sync
		if ( !_ntp.sync() )
		{
			elog( "CNodeServer::start_server(): NTP time sync failed" );
			return false;
		}
		log( "ntp::timestamp=%lld", _ntp.timestamp() );

		// peer server start
		_peer_server.start_server( host_data );

		// run thread io_service.run() 
		ios_run( thread_count );

		// callback timer
		_callback_timer.set_timer( timer_connect_peers, 60000, this );
		_callback_timer.set_timer( timer_ping, 1000, this );

		return true;
	}
	catch( std::exception& e )
	{
		elog( "CNodeServer::start_server(): exception! e=%s", e.what() );
	}

	return false;
}

bool CNodeServer::connect_peers( void )
{
	try
	{
		// connect to peers
		return _peer_server.connect_all_peers();
	}
	catch( std::exception& e )
	{
		elog( "CNodeServer::connect_peers(): exception! e=%s", e.what() );
	}

	return false;
}


bool CNodeServer::stop_server( void )
{
	try
	{
		// stop io worker
		_ios.stop();

		// stop peer server
		_peer_server.stop_server();

		// wait for all io thread
		for( auto& t : _io_threads )
			t->join();

		return true;
	}
	catch( std::exception& e )
	{
		elog( "CNodeServer::stop_server(): exception! e=%s", e.what() );
	}

	return false;
}

bool CNodeServer::ios_run( size_t thread_count )
{
	for( size_t i = 0 ; i < thread_count ; i++ )
	{
		std::unique_ptr< std::thread > thr( new std::thread( boost::bind( &boost::asio::io_service::run, &_ios ) ) );
		_io_threads.push_back( std::move( thr ) );
	}

	return true;
}

bool CNodeServer::append_peer( PeerData peer_data )
{
	return _peer_server.append_peer( peer_data );
}

void CNodeServer::onTimeOut( timer_id tid, void* pContext )
{
	switch( tid )
	{
	case timer_connect_peers:
		_peer_server.connect_all_peers();
		return;

	case timer_ping:
		{
			dlog( "CNodeServer[%d]: sending ping... ping_id=%d", node_id(), ++_ping_id );

			message_ping ping(_ping_id);
			_peer_server.sendall( &ping );
		}
		return;
	}
}


int CNodeServer::main( void )
{
	// callback timer
	_callback_timer.work();

	return 0 ;
}



// peer session callback
int CNodeServer::onPeerSessionConnected( CPeerSession* peer_session, bool connected )
{
	return 0;
}

int CNodeServer::onPeerSessionDisconnected( CPeer* peer, int reason )
{
	return 0;
}


int CNodeServer::onPeerSessionWrite( CPeer* peer, message* msg )
{
	return 0;
}


// peer message map for node message
#define		begin_peer_msg_map( _peer, _msg )		CPeer* __peer__=_peer; message* __message__=_msg; switch( _msg->msgid() ) {
#define		entry_peer_msg_map( _msg, _proc )		case _msg##_id: {	return _proc( __peer__, (_msg*)__message__ ); }
#define		end_peer_msg_map()						default: break; }


int CNodeServer::onPeerMessage( CPeer* peer, message* msg )
{
//	dlog( "CNodeServer::onPeerSessionMessage()" );

	begin_peer_msg_map( peer, msg )
		entry_peer_msg_map( message_text, onPeerMessageText )
		entry_peer_msg_map( message_ping, onPeerMessagePing )
		entry_peer_msg_map( message_pong, onPeerMessagePong )
	end_peer_msg_map()

	wlog( "CNodeServer::onPeerMessage() : unknown message" );

	return 0;
}



int CNodeServer::onPeerMessageText( CPeer* peer, message_text* msg )
{
	dlog( "CNodeServer[%d]::onPeerMessageText() : peer=%d, text=%s", node_id(), _peer_server.node_id(), msg->_text );

	return 0;
}

int CNodeServer::onPeerMessagePing( CPeer* peer, message_ping* msg )
{
//	dlog( "CNodeServer[%d]::onPeerMessagePing(): peer=%d, ping_id=%d", node_id(), peer->peer_id(), msg->_id );

	// ack pong
	message_pong ack( msg->_id );
	peer->send( &ack );

	return 0;
}

int CNodeServer::onPeerMessagePong( CPeer* peer, message_pong* msg )
{
	dlog( "CNodeServer[%d]::onPeerMessagePong(): peer=%d, ping_id=%d", node_id(), peer->peer_id(), msg->_id );

	return 0;
}


// peer client callback
int CNodeServer::onPeerClientConnected( CPeer* peer, bool connected )
{
	return 0;
}

int CNodeServer::onPeerClientDisconnected( CPeer* peer, int reason )
{
	return 0;
}

int CNodeServer::onPeerClientWrite( CPeer* peer, message* msg )
{
	return 0;
}

/*
int CNodeServer::onPeerClientMessage( CPeer* peer, message* msg )
{
	return 0;
}
*/


