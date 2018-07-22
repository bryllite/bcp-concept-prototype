# source files
set( node-server-lib-sources
	app_config.cpp app_config.hpp
	bridge_server.cpp bridge_server.hpp
	node_server.cpp node_server.hpp
	user_server.cpp user_server.hpp
	stdafx.cpp stdafx.hpp
	node_server_lib.hpp
)

add_library( node-server-lib STATIC ${node-server-lib-sources} )
target_link_libraries( node-server-lib
	bryllite-common
	bryllite-core
	${Boost_LIBRARIES}
	OpenSSL::Crypto
)
