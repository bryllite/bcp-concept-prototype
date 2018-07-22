# Boost 1.66+ required
find_package( Boost 1.66 REQUIRED COMPONENTS
	system
	filesystem
	date_time
	regex
	program_options
	unit_test_framework
	)

# OpenSSL 1.0.2+ required
find_package( OpenSSL REQUIRED )

#add_definitions( -DBOOST_TEST_DYN_LINK )
add_executable( unit_test 
	key_test.cpp sign_test.cpp
	)

target_link_libraries( unit_test
	bryllite-common 
	bryllite-core 
	${Boost_LIBRARIES} 
	OpenSSL::Crypto
	)

add_test( NAME unit_test COMMAND test )
