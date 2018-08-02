#pragma once

// application config
class app_config
{
public:
	enum { max_node_count = 100, invalid_node_id = invalid_peer_id };

	NodeID _node_id;				// node id
	size_t _max_node_count;			// max node count

	std::map< NodeID, PeerData > _node_peers;				// node <-> node address list
	std::map< NodeID, PeerData > _node_bridge_servers;		// node <-> bridge server address list
	std::map< NodeID, PeerData > _bridge_game_servers;		// bridge <-> game server address list
	std::map< NodeID, PeerData > _game_user_servers;		// game server <-> game user address list
	std::map< NodeID, PeerData > _node_user_servers;		// node <-> game user address list

	std::string _data_dir;		// data-dir
	std::string _log_dir;		// log-dir
	std::string _key_dir;		// key-dir

	std::string _config_file;	// config file

public:
	app_config();

	// load config
	bool loadConfig( void );

	// is file exists?
	bool file_exists( std::string filename );

	// create necessary path
	void create_directories( void );

public:
	NodeID node_id( void );

	// get each address list
	PeerData node_peer( NodeID node_id = invalid_node_id );			
	PeerData node_bridge_server( NodeID node_id = invalid_node_id );
	PeerData bridge_game_server( NodeID node_id = invalid_node_id );
	PeerData game_user_server( NodeID node_id = invalid_node_id );
	PeerData node_user_server( NodeID node_id = invalid_node_id );
	
};
