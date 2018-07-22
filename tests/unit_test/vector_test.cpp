#include <cstdio>
#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>

#include "bryllite-common.hpp"
#include "bryllite-core.hpp"


#include <boost/test/unit_test.hpp>



BOOST_AUTO_TEST_SUITE(test_suite_vector)


// _vector test
BOOST_AUTO_TEST_CASE(test_vector)
{
	bryllite::timer ts;
	size_t try_count = 0;

	while (++try_count > 0)
	{
		bryllite::key_pair _key_pair;
		_key_pair.generate_new();

		std::string address = _key_pair.address();
		std::string temp = address.substr(0, 5);
		//		std::transform( temp.begin(), temp.end(), temp.begin(), std::tolower );

		if (strncmp(temp.c_str(), "1jade", 5) == 0) {
			bryllite::colors::set(bryllite::colors::light_green);
			std::cout << "[100%] address=" << address << ", laptime=" << (ts.laptime() / 1000) << "(s), try=" << try_count << std::endl;
			bryllite::colors::set(bryllite::colors::_default);
			break;
		}

		if (strncmp(temp.c_str(), "1jad", 4) == 0) {
			bryllite::colors::set(bryllite::colors::light_yellow);
			std::cout << "[75%] address=" << address << ", laptime=" << (ts.laptime() / 1000) << "(s), try=" << try_count << std::endl;
			bryllite::colors::set(bryllite::colors::_default);
			continue;
		}

		if (strncmp(temp.c_str(), "1ja", 3) == 0) {
			bryllite::colors::set(bryllite::colors::light_cyan);
			std::cout << "[50%] address=" << address << ", laptime=" << (ts.laptime() / 1000) << "(s), try=" << try_count << std::endl;
			bryllite::colors::set(bryllite::colors::_default);
			continue;
		}

		if (strncmp(temp.c_str(), "1j", 2) == 0) {
			bryllite::colors::set(bryllite::colors::_default);
			std::cout << "[25%] address=" << address << ", laptime=" << (ts.laptime() / 1000) << "(s), try=" << try_count << std::endl;
			continue;
		}
	}
}


BOOST_AUTO_TEST_SUITE_END()
