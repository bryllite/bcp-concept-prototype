set( game-client-sources 
	main.cpp 
	game_client.cpp game_client.hpp
)

add_executable( game-client ${game-client-sources} )
target_link_libraries( game-client 
	bryllite-common 
	bryllite-core 
	${Boost_LIBRARIES} 
)
