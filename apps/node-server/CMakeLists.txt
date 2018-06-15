set( node-server-sources 
	main.cpp app_config.hpp app_config.hpp bridge_client.hpp node_message.inl
	node_account.cpp node_account.hpp
	block_chain.cpp block_chain.hpp
	node_session.cpp node_session.hpp
	node_server.cpp node_server.hpp
	_vector.cpp _vector.hpp
)

add_executable( node-server ${node-server-sources} )
target_link_libraries( node-server 
	bryllite-common 
	bryllite-core 
	${Boost_LIBRARIES} 
	OpenSSL::Crypto
)
