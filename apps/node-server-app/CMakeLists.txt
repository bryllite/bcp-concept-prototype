set( node-server-app-sources
	main.cpp
)

include_directories(../node-server-lib)

add_executable( node-server-app ${node-server-app-sources} )
target_link_libraries( node-server-app
	bryllite-common
	bryllite-core
	node-server-lib
	${Boost_LIBRARIES}
	OpenSSL::Crypto
)
