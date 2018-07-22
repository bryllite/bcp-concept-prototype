#pragma once


// account class
class CAccount : public CKeyPair
{
public:
	CAccount();
	CAccount( const std::string& keyFile );

	CKeyPair& keyPair( void );
};
