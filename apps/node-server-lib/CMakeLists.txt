# BCP SOURCE FILES
file(GLOB_RECURSE BCP_SOURCE_FILES "bcp/*.cpp" "bcp/*.hpp" )

# source files
set( node-server-lib-sources
	app_config.cpp app_config.hpp
	bridge_server.cpp bridge_server.hpp
	user_server.cpp user_server.hpp
	${BCP_SOURCE_FILES}
	node_server.cpp node_server.hpp
	stdafx.cpp stdafx.hpp
	node_server_lib.hpp
)

# BCP FILTERS
source_group("Bcp Files" FILES ${BCP_SOURCE_FILES} )

add_library( node-server-lib STATIC ${node-server-lib-sources} )
target_link_libraries( node-server-lib
	bryllite-common
	bryllite-core
	${Boost_LIBRARIES}
	OpenSSL::Crypto
)
