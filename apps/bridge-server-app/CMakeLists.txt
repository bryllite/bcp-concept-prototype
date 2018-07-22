set( bridge-server-app-sources
	main.cpp
)

include_directories(../bridge-server-lib)

add_executable( bridge-server-app ${bridge-server-app-sources} )
target_link_libraries( bridge-server-app
	bryllite-common
	bryllite-core
	bridge-server-lib
	${Boost_LIBRARIES}
	OpenSSL::Crypto
)
