add_executable( bcp_mono 
	main.cpp 
	node.cpp node.hpp
	node_manager.cpp node_manager.hpp
	node_message.inl 
	_vector.cpp _vector.hpp
)

target_link_libraries( bcp_mono 
	bryllite-common 
	bryllite-core 
	${Boost_LIBRARIES} 
	OpenSSL::Crypto
)
