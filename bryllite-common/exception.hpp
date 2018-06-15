/*
@file: exception.hpp
@desc: bryllite exception class
@author: jade@bryllite.com
@date: 2018-05-14
@note: bryllite exception class
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_EXCEPTION_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_EXCEPTION_HPP_

#include <string>

#include "utils.hpp"

// namespace bryllite
namespace bryllite {


class exception
{
public:
	exception();
	exception( int errorCode );
	exception( std::string errorMessage );
	exception( int errorCode, std::string errorMessage );
	virtual ~exception();

	virtual int code( void );
	virtual const char* message( void );
	virtual const char* what( void );

protected:
	static const int E_SUCCESS = 0;
	int _errorCode ;			// erorr code
	std::string _errorMessage ;	// error message
};


}; // namespace bryllite

#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_EXCEPTION_HPP_
