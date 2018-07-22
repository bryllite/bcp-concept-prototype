# source files
set( game-client-lib-sources
	app_config.cpp app_config.hpp
	node_client.cpp node_client.hpp
	user_client.cpp node_client.hpp
	game_client.cpp game_client.hpp
	stdafx.cpp stdafx.hpp
	game_client_lib.hpp
)

add_library( game-client-lib STATIC ${game-client-lib-sources} )
target_link_libraries( game-client-lib
	bryllite-common
	bryllite-core
	${Boost_LIBRARIES}
	OpenSSL::Crypto
)
