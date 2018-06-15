#add_executable( test1 test1.cpp )
#target_link_libraries( test1 bryllite-common bryllite-core ${Boost_LIBRARIES} )

#add_executable( test-lockable lock.cpp )
#target_link_libraries( test-lockable bryllite-common ${Boost_LIBRARIES} )

#add_executable( echoserver echoserver.cpp )
#target_link_libraries( echoserver bryllite-common ${Boost_LIBRARIES} )

#add_executable( echoclient echoclient.cpp )
#target_link_libraries( echoclient bryllite-common ${Boost_LIBRARIES} )

#add_executable( udpserver udpserver.cpp )
#target_link_libraries( udpserver bryllite-common bryllite-core ${Boost_LIBRARIES} )

#add_executable( udpclient udpclient.cpp )
#target_link_libraries( udpclient bryllite-common bryllite-core ${Boost_LIBRARIES} )

#add_executable( openssl openssl_test.cpp )
#target_link_libraries( openssl 
#	bryllite-common 
#	bryllite-core 
#	${Boost_LIBRARIES} 
#	${OPENSSL_LIBRARIES}
#)

#add_executable( secp256k1 secp256k1.cpp )
#target_link_libraries( secp256k1 
#	bryllite-common 
#	bryllite-core 
#	${Boost_LIBRARIES} 
#	OpenSSL::Crypto
#)

#add_executable( ecc_test 
#	ecc_test.cpp 
#)
#target_link_libraries( ecc_test 
#	bryllite-common 
#	bryllite-core 
#	${Boost_LIBRARIES} 
#	OpenSSL::Crypto
#)

#add_executable( bcp 
#	bcp.cpp 
#)
#target_link_libraries( bcp 
#	bryllite-common 
#	bryllite-core 
#	${Boost_LIBRARIES} 
#	OpenSSL::Crypto
#)

add_executable( key_test 
	key_test.cpp 
)
target_link_libraries( key_test 
	bryllite-common 
	bryllite-core 
	${Boost_LIBRARIES} 
	OpenSSL::Crypto
)

add_subdirectory( bcp_mono )


