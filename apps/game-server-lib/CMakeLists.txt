# source files
set( game-server-lib-sources
	app_config.cpp app_config.hpp
	user_server.cpp user_server.hpp
	game_server.cpp game_server.hpp
	stdafx.cpp stdafx.hpp
	game_server_lib.hpp
)

add_library( game-server-lib STATIC ${game-server-lib-sources} )
target_link_libraries( game-server-lib
	bryllite-common
	bryllite-core
	${Boost_LIBRARIES}
	OpenSSL::Crypto
)
