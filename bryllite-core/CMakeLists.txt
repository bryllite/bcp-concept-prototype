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
	address.cpp address.hpp
	publickey.cpp publickey.hpp
	privatekey.cpp privatekey.hpp
	keypair.cpp keypair.hpp
	coin.cpp coin.hpp
	account.cpp account.hpp
	secret.cpp secret.hpp
	leveldb.cpp leveldb.hpp
	block_header.cpp block_header.hpp
	transaction.cpp transaction.hpp
	block.cpp block.hpp
	block_chain.cpp block_chain.hpp
	socket_buffer.cpp socket_buffer.hpp
	peer_data.cpp peer_data.hpp
	peer.cpp peer.hpp
	peer_client.cpp peer_client.hpp
	peer_server.cpp peer_server.hpp
	peer_session.cpp peer_session.hpp
	bcp_timer.cpp bcp_timer.hpp
	bcp_server.cpp bcp_server.hpp
	bryllite-core.cpp bryllite-core.hpp
	stdafx.cpp stdafx.hpp
)

add_library( bryllite-core STATIC ${bryllite-core-sources} )
target_link_libraries( bryllite-core
	bryllite-common
	${Boost_LIBRARIES}
	OpenSSL::Crypto
)
