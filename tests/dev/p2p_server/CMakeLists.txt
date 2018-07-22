add_executable( p2p_server
	main.cpp 
	p2p_server.cpp p2p_server.hpp
)

target_link_libraries( p2p_server
	bryllite-common 
	bryllite-core 
	${Boost_LIBRARIES} 
	OpenSSL::Crypto
)
