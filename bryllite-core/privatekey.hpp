#pragma once

// private key class
class CPrivateKey : public uint256
{
public:
	CPrivateKey();
	CPrivateKey( const uint256& key );
	CPrivateKey( std::string key );

	operator uint256&() const;
	operator uint256() const;

	std::string str( void );
};
