#pragma once

#include <bryllite-common.hpp>
#include "secp256r1.hpp"

// namespace bryllite
namespace bryllite {

	// public key
	class public_key
	{
		friend class key_pair;
	public:
		uint264 _key;
		std::string _address;

	public:
		public_key() ;
		public_key( const uint264& key ) ;
		public_key( std::string key ) ;

		operator uint264&() ;

		std::string address( void ) ;

	protected:
		std::string toAddress( void );
	};

	// private key
	class private_key
	{
	public:
		uint256 _key;

	public:
		private_key();
		private_key( uint256 key );
		private_key( std::string key );

		operator uint256&();
	};

	// key pair
	class key_pair
	{
	public:
		private_key _private;
		public_key _public;

		key_pair();

		bool generate_new( void );
		bool read( std::string keyFile );
		bool write( std::string keyFile );

		uint256 get_private_key( void );
		uint264 get_public_key( void );
		std::string address( void );
	};

	// signature
	class signature
	{
	public:
		uint512 _sig;

		signature();
		signature( uint512 sig );
		signature( std::string str );

		operator uint512&();
	};

	// public key to address(30 bytes)
	std::string public_key_to_address( uint264 public_key );

	// account address ( 30bytes )
	struct address
	{
		enum { length = 30 };
		unsigned char _data[length+2];

		address();
		address( std::string str );
		std::string str( void );

		operator const char*();
	};

};//namespace bryllite
