# source files
set( bridge-server-lib-sources
	app_config.cpp app_config.hpp
	game_server.cpp game_server.hpp
	bridge_server.cpp bridge_server.hpp
	stdafx.cpp stdafx.hpp
	bridge_server_lib.hpp
)

add_library( bridge-server-lib STATIC ${bridge-server-lib-sources} )
target_link_libraries( bridge-server-lib
	bryllite-common
	bryllite-core
	${Boost_LIBRARIES}
	OpenSSL::Crypto
)
