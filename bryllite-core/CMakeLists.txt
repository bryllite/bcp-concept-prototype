# source files
set( bryllite-core-sources
	message.inl
	tcp_client.cpp tcp_client.hpp
	tcp_server.cpp tcp_server.hpp
	udp_server.cpp udp_server.hpp
	peer_host.cpp peer_host.hpp
	uint256.cpp uint256.hpp
	utilstrencodings.cpp utilstrencodings.hpp tinyformat.hpp
	secp256r1.cpp secp256r1.hpp
	ripemd160.cpp ripemd160.hpp
	base58.cpp base58.hpp
	sha256.cpp sha256.hpp
	block.cpp block.hpp
	transaction.cpp transaction.hpp
	key.cpp key.hpp
	bryllite-core.cpp bryllite-core.hpp
)

add_library( bryllite-core STATIC ${bryllite-core-sources} )
target_link_libraries( bryllite-core 
	bryllite-common 
	${Boost_LIBRARIES} 
	OpenSSL::Crypto
)

