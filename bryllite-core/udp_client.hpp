#pragma once

#include <bryllite-common.hpp>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "message.inl"

// namespace bryllite::net
namespace bryllite { namespace net {

// udp async client
class udp_client 
{
	using udp = boost::asio::ip::udp;

protected:
	boost::asio::io_service _ios;
	udp::socket _socket;
	udp::endpoint _remote_endpoint, _read_endpoint ;
	boost::asio::streambuf _read_buffer, _write_buffer;	// read/write buffer

	std::unique_ptr< std::thread > _io_thread;	// io worker thread for io_service

public:
	udp_client() 
		: _socket( _ios, udp::endpoint(udp::v4(), 0) ) 
	{
	};

	bool udp_client_start( const char* host, unsigned short port )
	{
		_remote_endpoint = udp::endpoint( boost::asio::ip::address::from_string( host ), port );

		if ( do_read() ) {
			_io_thread.reset( new std::thread( boost::bind( &boost::asio::io_service::run, &_ios ) ) );
			return true ;
		}

		return false;
	};

	bool udp_client_stop( void )
	{
		try
		{
			_ios.stop();
			_io_thread->join();
			return true;
		}
		catch( std::exception& e )
		{
			elog( "udp_client::udp_client_stop() >> exception! e='%s'", e.what() );
		}
		catch( ... )
		{
			elog( "udp_client::udp_client_stop() >> unknown exception!" );
		}

		return false;
	};

	bool udp_sendto( message* msg ) {
		return udp_sendto( _remote_endpoint, msg );
	};
	bool udp_sendto( const char* data, size_t size ) {
		return udp_sendto( _remote_endpoint, data, size );
	};
	bool udp_sendto( udp::endpoint endpoint, message* msg ) {
		return udp_sendto( endpoint, (const char*)msg, msg->size() );
	};
	bool udp_sendto( udp::endpoint endpoint, const char* data, size_t size ) 
	{
		try
		{
/*
			// copy datagram to write buffer
			std::ostreambuf_iterator<char> out( &_write_buffer );
			std::copy( data, data + size, out );
		
			_socket.async_send_to( _write_buffer.data(), endpoint, boost::bind( &udp_client::_on_send_to, this, endpoint, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
*/
			_socket.send_to( boost::asio::buffer( data, size ), endpoint );
			return true;
		}
		catch( std::exception& e )
		{
			elog( "udp_client::udp_sendto() >> exception! e='%s'", e.what() ) ;
		}
		catch( ... )
		{
			elog( "udp_client::udp_sendto() >> unknown exception!" );
		}

		return false;
	};

protected:
	bool do_read( void )
	{
		try
		{
			boost::asio::streambuf::mutable_buffers_type buf = _read_buffer.prepare( max_message_length );
			_socket.async_receive_from( buf, _read_endpoint, 
										boost::bind( &udp_client::_on_receive_from, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
			return true;
		}
		catch( std::exception& e )
		{
			elog( "udp_client::do_read() >> exception! e='%s'", e.what() );
		}
		catch( ... )
		{
			elog( "udp_client::do_read() >> unknown exception!" );
		}

		return false;
	};

	void _on_send_to( udp::endpoint endpoint, const boost::system::error_code& error, size_t bytes_transferred )
	{
		onUdpClientWrite( endpoint, bytes_transferred );

		_read_buffer.consume( bytes_transferred );
	};

	void _on_receive_from( const boost::system::error_code& error, size_t bytes_transferred )
	{
		if ( !error && bytes_transferred > 0 )
		{
			udp::endpoint& ep = _read_endpoint;

			_read_buffer.commit( bytes_transferred );

			// read buffer message
			const char* data = boost::asio::buffer_cast< const char* >( _read_buffer.data() );
			message* msg = ( message* )data;
			if( msg && msg->valid() )
			{
				// call message receive callback handler
				onUdpClientMessage( ep, msg );
			}

			_read_buffer.consume( bytes_transferred );
		}

		do_read();
	};


	virtual void onUdpClientWrite( udp::endpoint endpoint, size_t bytes_transferred ) {
		log( "udp_client::onUdpClientWrite(): bytes_tranferred=%d", bytes_transferred );
	};

	virtual void onUdpClientMessage( udp::endpoint endpoint, message* msg ) {
		log( "udp_client::onUdpClientMessage(): msg.id=%d, msg.size=%d", msg?msg->msgid():0, msg?msg->size():0 );
	};
};


}}; //namespace bryllite::net