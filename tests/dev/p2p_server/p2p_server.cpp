#include "p2p_server.hpp"

#include "peer_message.inl"




p2p_server::p2p_server() : _acceptor(_ios ), _accept_session(nullptr)
{
};

// new session factory
p2p_session* p2p_server::new_session( void )
{
	return new p2p_session(_ios);
};

// close_session
void p2p_server::close_session( p2p_session* session )
{
	if( session )
	{
		onDisconnected( session );
		session->close();
	}

	// remove session if closed 1 sec over
//	remove_session_if( 1000 );
};

// remove_session if expired
void p2p_server::remove_session_if( time_t expired )
{
	_sessions.erase(
		std::remove_if( _sessions.begin(), _sessions.end(), [expired]( std::shared_ptr<p2p_session> session ) -> bool {
			return ( session->expired( expired ) );
		}), _sessions.end()	
	);
};


CPeer* p2p_server::find_peer( std::string peer_id )
{
	for( auto& p : _peers )
	{
		std::string id = p.first;
		if ( peer_id.compare(id) == 0 )
			return p.second.get();
	}

	return nullptr;
};

bool p2p_server::append_peer( std::string peer_id, std::string host, unsigned short port )
{
	if ( find_peer(peer_id) ) return false;

	peer_info peer( peer_id, host, port );
	std::unique_ptr< CPeer > p( new CPeer( _ios, peer, _me ) );
	_peers.insert( std::make_pair( peer_id, std::move(p) ) );

	return true;
};


bool p2p_server::start_server( std::string node_id, std::string host, unsigned short port, int io_thread_cnt )
{
	boost::system::error_code error;
	_local_endpoint = tcp::endpoint( tcp::v4(), port );
	_me = peer_info( node_id, host, port );

	try
	{
		// open acceptor socket
		_acceptor.open( _local_endpoint.protocol(), error );
		if( error )
		{
			elog( "p2p_server::tcp_start() >> _acceptor.open() failed. port=%d, error='%s'(%d)", port, error.message().c_str(), error.value() );
			return false;
		}

		// bind & listen
		_acceptor.set_option( tcp::acceptor::reuse_address( true ) );
		_acceptor.bind( _local_endpoint, error );
		if( error )
		{
			elog( "p2p_server::tcp_start() >> _acceptor.bind() failed. port=%d, error='%s'(%d)", port, error.message().c_str(), error.value() );
			return false;
		}
		_acceptor.listen();

		// accept 
		accept();

		// run io threads
		for( int i = 0; i<io_thread_cnt; i++ )
		{
			std::unique_ptr< std::thread > t( new std::thread( boost::bind( &boost::asio::io_service::run, &_ios ) ) );
			_io_threads.push_back( std::move( t ) );
		}

		_ntp.sync();

		return true;
	}
	catch( std::exception& e )
	{
		elog( "p2p_server::tcp_start() >> exception! e='%s'", e.what() );
	}
	catch( ... )
	{
		elog( "p2p_server::tcp_start() >> unknown exception!" );
	}

	return false;
};

bool p2p_server::connect_to_peers( void )
{
	for( auto& p : _peers )
	{
		connect_to_peer( p.second.get() );
	}

	return true;
};

bool p2p_server::connect_to_peer( CPeer* peer )
{
	if ( peer->connected() ) return false;

	return peer->connect_peer();
};


bool p2p_server::stop_server( void )
{
	try
	{
		// stop io service
		_ios.stop();

		// wait for all io thread
		for( auto& t : _io_threads )
			t->join();

		if ( _accept_session )
		{
			delete _accept_session;
			_accept_session = nullptr;
		}

		return true;
	}
	catch( std::exception& e )
	{
		elog( "p2p_server::tcp_stop() >> exception! e='%s'", e.what() );
	}
	catch( ... )
	{
		elog( "p2p_server::tcp_stop() >> unknown exception!" );
	}

	return false;
};

// sending message
void p2p_server::sendto( p2p_session* session, message* msg )
{
	return sendto( session, (byte*)msg, msg->size() );
};

void p2p_server::sendto( p2p_session* session, byte* data, size_t len )
{
	_assert( session != nullptr, "session=%x", session );

	session->write( data, len );
};

// write message to all client ( except for except_session )
void p2p_server::sendall( message* msg )
{
	sendall( (byte*)msg, msg->size() );
};

void p2p_server::sendall( byte* data, size_t len )
{
	smartlock( this );

	for( auto& session : _sessions ) 
	{
		if( session->active() ) 
			sendto( session.get(), data, len );
	}
}

// accept
bool p2p_server::accept( void )
{
	_accept_session = new_session();
	_acceptor.async_accept( _accept_session->socket(), boost::bind( &p2p_server::handle_accept, this, _accept_session, boost::asio::placeholders::error ) );

	return true;
};

// accept procedure
void p2p_server::handle_accept( p2p_session* session, const boost::system::error_code& error )
{
	if( !error ) 
	{
		log( "new connection accepted" );

		session->active( true );

		if( session ) {
			smartlock( this );
			std::shared_ptr< p2p_session > s( session );
			_sessions.push_back( std::move( s ) );
		}

		session_start( session );
		onAccept( session );
	}
	else {
		elog( "handle_accept() failed. error=%d", error );
		close_session( session );
	}

	accept();
};

// session start with read header
void p2p_server::session_start( p2p_session* session ) 
{
	session_read_header( session );
};


// read header
void p2p_server::session_read_header( p2p_session* session ) 
{
	_assert( session != nullptr, "session=%x", session );

	// read buffer
	byte* buffer = session->next_read_buffer();

	// read header
	boost::asio::async_read( session->socket(), boost::asio::buffer( buffer, session->read_buffer_length() ),
							 boost::asio::transfer_exactly( sizeof( message ) ),
							 boost::bind( &p2p_server::handle_session_read_header, this, session, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
};

// read header procedure
void p2p_server::handle_session_read_header( p2p_session* session, const boost::system::error_code& error, size_t bytes_transferred ) 
{
	_assert( session != nullptr, "session=%x", session );

	if( error ) {
		wlog( "p2p_server::handle_read_header() failed. error=%d, bytes_transferred=%d", error, bytes_transferred );
		close_session( session );
		return;
	}

	smartlock( session );

	// read buffer
	byte* buffer = session->read_buffer();

	size_t len = 0;
	message* header = reinterpret_cast<message*>( buffer );
	if( !header->valid() ) {
		wlog( "invalid message header: id=%d, size=%d, bytes_transferred=%d", header->msgid(), header->size(), bytes_transferred );
		close_session( session );
		return;
	}

	session_read_body( session, header );
};

// read payload
void p2p_server::session_read_body( p2p_session* session, message* header ) 
{
	_assert( session != nullptr, "session=%x", session );

	byte* buffer = (byte*)header + sizeof(message);

	// read payload
	boost::asio::async_read( session->socket(),
							 boost::asio::buffer( buffer, session->read_buffer_length() - sizeof(message) ),
							 boost::asio::transfer_exactly( header->payload_size() ),
							 boost::bind( &p2p_server::handle_session_read_body, this, session, header, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
};

// read payload procedure
void p2p_server::handle_session_read_body( p2p_session* session, message* header, const boost::system::error_code& error, size_t bytes_transferred ) 
{
	_assert( session != nullptr, "session=%x", session );

	if( error ) {
		wlog( "handle_session_read_body() failed. error=%d, bytes_transferred=%d", error, bytes_transferred );
		close_session( session );
		return;
	}

	if ( header->size() < bytes_transferred + sizeof(message) )
	{
		wlog( "handle_session_read_body() failed. header.size()=%d, bytes_transferred=%d", header->size(), bytes_transferred );
		close_session( session );
		return ;
	}

	smartlock( session );

	message* msg = header;
	if ( !msg->valid() )
	{
		wlog( "handle_session_read_body(): invalid message" );
		close_session( session );
		return ;
	}

	// message procedure
	onMessage( session, msg );

	// wait for other message
	session_read_header( session );
};

// new connection
void p2p_server::onAccept( p2p_session* session ) 
{
	log( "p2p_server::onAccept()" );
};

// disconnected
void p2p_server::onDisconnected( p2p_session* session ) 
{
	log( "p2p_server::onDisconnected()" );
};




// message received
void p2p_server::onMessage( p2p_session* session, message* msg ) 
{
	log( "p2p_server::onMessage() >> message.size=%d", msg->size() );


};





