set( game-client-app-sources
	main.cpp
)

include_directories(../game-client-lib)

add_executable( game-client-app ${game-client-app-sources} )
target_link_libraries( game-client-app
	bryllite-common
	bryllite-core
	game-client-lib
	${Boost_LIBRARIES}
	OpenSSL::Crypto
)
