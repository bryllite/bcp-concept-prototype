set( bridge-server-sources 
	main.cpp 
	bridge_server.cpp bridge_server.hpp
)

add_executable( bridge-server ${bridge-server-sources} )
target_link_libraries( bridge-server 
	bryllite-common 
	bryllite-core 
	${Boost_LIBRARIES} 
)
