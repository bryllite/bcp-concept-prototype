add_executable( node-server-mono
	main.cpp
	bcp_server.cpp bcp_server.hpp
	node_message.inl
	node_server.cpp node_server.hpp
	node_manager.cpp node_manager.hpp
	node_message.inl
	stdafx.cpp stdafx.hpp
)

target_link_libraries( node-server-mono
	bryllite-common
	bryllite-core
	${Boost_LIBRARIES}
	OpenSSL::Crypto
)
