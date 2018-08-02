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

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstdarg>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <chrono>
#include <ctime>
#include <time.h>
#include <map>
#include <vector>
#include <array>

#include <thread>
#include <atomic>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

// top level header
#include "version.hpp"
#include "types.hpp"
#include "assert.hpp"
#include "utils.hpp"
#include "exception.hpp"
#include "singleton.hpp"
#include "lockable.hpp"

// dependent header
#include "clog.hpp"
#include "threadable.hpp"
#include "fsm.hpp"
#include "timer.hpp"

// namespace bryllite
namespace bryllite {
	const char* version( void ) ;
	unsigned short version_major( void ) ;
	unsigned short version_minor( void ) ;
	unsigned short version_patch( void ) ;
	unsigned long version_number( void ) ;
}; //namespace bryllite

#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_BRYLLITE_COMMON_HPP_

