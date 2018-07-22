add_executable( node-server-test
	node_server.cpp node_server.hpp
	main.cpp 
	stdafx.cpp stdafx.hpp
)

target_link_libraries( node-server-test
	bryllite-common 
	bryllite-core 
	${Boost_LIBRARIES} 
	OpenSSL::Crypto
)
