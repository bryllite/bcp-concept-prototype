#ifndef __MESSAGE_HEADER__
#define __MESSAGE_HEADER__

const unsigned short tcp_port = 8000;
const unsigned short udp_port = 9000;

enum { max_length = 4096 } ;

#ifdef _MSC_VER
// 2bytes align for packet
#pragma pack(push)
#pragma pack(2)
#endif // _MSC_VER


// message header
class message_header
{
public:
	enum { max_msg_length = 4096 };

	message_header() : _msgid(0), _size( sizeof( message_header ) ), _signature( "BRC" ) {
	};

	message_header( unsigned short msgid ) : _msgid(msgid), _size( sizeof(message_header) ), _signature("BRC") {
	};

	unsigned short msgid( void ) { return _msgid ; } ;
	void msgid( unsigned short msgid ) { _msgid = msgid ; } ;

	size_t size( void ) { return _size; };
	void size( size_t size ) { _size = size; };
	
	bool valid( void ) {
		return ( _signature[0] == 'B' && _signature[1] == 'R' && _signature[2] == 'C' && _signature[3] == 0 )
			&& ( _size >= sizeof( message_header ) && _size <= max_msg_length );
	};

protected:
	char _signature[4];
	size_t _size;
	unsigned short _msgid;
};


class message : public message_header
{
public:
	message() {
		memset( m_payload, 0, sizeof( m_payload ) ) ;
	};

	message( unsigned short msgid ) : message_header( msgid ) {
	};

	message( std::string str ) : message() {
		set( str );
	};

	message( unsigned short msgid, std::string str ) : message(msgid) {
		set( str );
	};

	void set( std::string str ) {
		set( str.c_str(), str.length() );
	};

	void set( const char* str, size_t size ) {
		_assert( size > 0 && size < max_msg_length, "message size=%d", size );
		if( nullptr != str ) {
			memcpy( m_payload, str, size );
			_size = size + sizeof( message_header );
		}
	};
	const char* payload( void ) { return m_payload; };

	char* buffer( void ) { return ( char* )this; };

protected:
	char m_payload[max_msg_length + 1];
};


#endif // __MESSAGE_HEADER__