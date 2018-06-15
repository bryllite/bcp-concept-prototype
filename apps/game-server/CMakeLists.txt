set( game-server-sources 
	main.cpp bridge_client.hpp
	game_server.cpp game_server.hpp
)

add_executable( game-server ${game-server-sources} )
target_link_libraries( game-server 
	bryllite-common 
	bryllite-core 
	${Boost_LIBRARIES} 
)
