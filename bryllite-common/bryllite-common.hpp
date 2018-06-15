/*
@file: bryllite-common.hpp
@desc: bryllite common library header for including all
@author: jade@bryllite.com
@date: 2018-05-14
@note: common header for all
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_BRYLLITE_COMMON_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_BRYLLITE_COMMON_HPP_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#endif //_WIN32

#include "version.hpp"
//#include "enum.hpp"
#include "utils.hpp"
#include "types.hpp"
#include "singleton.hpp"
#include "assert.hpp"
#include "exception.hpp"
#include "lockable.hpp"
#include "timer.hpp"
#include "log.hpp"
#include "threadable.hpp"
#include "fsm.hpp"
//#include "nlohmann/json.hpp"


// namespace bryllite
namespace bryllite {
	const char* version( void ) ;
	unsigned short version_major( void ) ;
	unsigned short version_minor( void ) ;
	unsigned short version_patch( void ) ;
	unsigned long version_number( void ) ;
}; //namespace bryllite

#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_BRYLLITE_COMMON_HPP_

