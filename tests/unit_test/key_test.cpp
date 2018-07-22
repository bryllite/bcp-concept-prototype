#define	BOOST_TEST_MODULE	Bryllite Test suite

#include <cstdio>
#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"


#include <boost/test/unit_test.hpp>



BOOST_AUTO_TEST_SUITE(test_suite_key)


// how long time elapsed to generate 4 bytes decorated address
BOOST_AUTO_TEST_CASE( test_find_address )
{
	bryllite::timer ts;
	size_t try_count = 0;

	while( ++try_count > 0 )
	{
		CKeyPair keyPair;
		keyPair.newKeyPair();

		std::string address = keyPair.address().str();
		std::string temp = address.substr( 0, 5 );
//		std::transform( temp.begin(), temp.end(), temp.begin(), std::tolower );

		if( strncmp( temp.c_str(), "1jade", 5 ) == 0 ) {
			bryllite::clog << Color::GREEN << "[100%] address=" << address << ", laptime=" << ( ts.laptime() / 1000 ) << "(s), try=" << try_count << bryllite::endl;
			break;
		}

		if( strncmp( temp.c_str(), "1jad", 4 ) == 0 ) {
			bryllite::clog << Color::YELLOW << "[75%] address=" << address << ", laptime=" << ( ts.laptime() / 1000 ) << "(s), try=" << try_count << bryllite::endl;
			continue;
		}

		if( strncmp( temp.c_str(), "1ja", 3 ) == 0 ) {
			bryllite::clog << Color::CYAN << "[50%] address=" << address << ", laptime=" << ( ts.laptime() / 1000 ) << "(s), try=" << try_count << bryllite::endl;
			continue;
		}

		if( strncmp( temp.c_str(), "1j", 2 ) == 0 ) {
			bryllite::clog << Color::gray << "[25%] address=" << address << ", laptime=" << ( ts.laptime() / 1000 ) << "(s), try=" << try_count << bryllite::endl;
			continue;
		}
	}
}


// is valid address?
bool is_valid_address(std::string address)
{
	if (address.length() < CAddress::min_length) return false;
	if (address.length() > CAddress::max_length) return false;

	std::vector<byte> vch;
	return decode_base58_check(address, vch);
}

BOOST_AUTO_TEST_CASE(test_key)
{
	std::map< std::string, CPrivateKey > keyMap;

	size_t max_count = 10000;	
	size_t idx = 0;
	while( idx++ < max_count )
	{
		// create key pair
		CKeyPair keyPair;
		keyPair.newKeyPair();

		// key address
		std::string address = keyPair.address().str();
		std::cout << "[" << idx << "]: address=" << address << ", length=" << address.length() << std::endl;

		// is valid address?
		BOOST_CHECK(is_valid_address(address));

		// collision check
		BOOST_CHECK(keyMap.count(address) <= 0);

		// insert address to key map
		keyMap.insert( std::make_pair( address, keyPair.privateKey() ) );
	}
}


// generate random string ( 100 - 199 )
std::string generate_random_string(void)
{
	std::stringstream ss;

	size_t length = 100 + (rand() % 100);
	for (size_t i = 0; i < length; i++)
	{
		ss << bryllite::byte_to_hex((byte)(rand() % 256));
	}

	return ss.str();
}

BOOST_AUTO_TEST_CASE(test_sign)
{
	size_t idx = 0;
	size_t max_count = 10000;

	while (idx++ < max_count)
	{
		CKeyPair keyPair;
		keyPair.newKeyPair();

		std::string str = generate_random_string();

		uint512 sig;

		// sign
		bryllite::do_sign(keyPair.privateKey(), bryllite::sha256(str), sig);

		// verify
		bool verify = bryllite::do_verify(keyPair.publicKey(), bryllite::sha256(str), sig);

		BOOST_CHECK(verify);
	}
}


BOOST_AUTO_TEST_SUITE_END()


