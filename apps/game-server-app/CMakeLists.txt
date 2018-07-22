set( game-server-app-sources
	main.cpp
)

include_directories(../game-server-lib)

add_executable( game-server-app ${game-server-app-sources} )
target_link_libraries( game-server-app
	bryllite-common
	bryllite-core
	game-server-lib
	${Boost_LIBRARIES}
	OpenSSL::Crypto
)
