/*
@file: log.hpp
@desc: custom logmanager
@author: jade@bryllite.com
@date: 2018-05-14
@note: log manager
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_LOG_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_LOG_HPP_

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "singleton.hpp"
#include "lockable.hpp"
#include "colors.hpp"
#include "timer.hpp"


// namespace bryllite
namespace bryllite {

// log attr
struct log_attr {
	enum type {
		none = 0x0000,
		file = 0x0001,
		source = 0x0002,
		time = 0x0004,
		_default = time|file,
		all = 0xffff,
	};
};

// log level
struct log_level {
	enum type {
		none = 0x0000,
		error = 0x0001,
		warning = 0x0002,
		info = 0x0004,
		debug = 0x0008,
		user = 0x0010,
		dev = error | warning | info | debug | user,
		release = error | warning | info,
		all = 0xffff,
	};
};


// logmanager class
class logmanager final : public lockable, public singleton< logmanager >
{
	friend class singleton< logmanager > ;

private:
	// logmanager ctor
	logmanager();
	
	// logmanager dtor
	virtual ~logmanager();

public:

	// initialize log
	bool initialize(const char* logFileName = nullptr, log_attr::type logAttr = log_attr::_default, log_level::type logFilter = log_level::all);

	// native log message ( nothing but message )
	void _native(const char* format, ...);

	// write log ( to screen or file )
	void _log( const char* filename, int line, log_level::type level, const char* fmt, ... ) ;

private:
	// log message to file
	void write_file( std::string message ) ;

	// get current log file name ( $DATE replace to current date code )
	std::string logFileName(void);

private:
	log_attr::type m_logAttr ;			// log attr
	log_level::type m_logFilter ;		// log filter
	std::string m_logFileName ;			// log filename ( e.g. "bryllite-$DATE.log" $DATE replace to current datecode "bryllite-2018-05-17.log" )
} ;


}; // namespace bryllite


#define		log_manager()			bryllite::logmanager::instance()

#define		elog( fmt, ... )		log_manager()->_log( __FILENAME__, __LINE__, bryllite::log_level::error, fmt, ##__VA_ARGS__ )
#define		wlog( fmt, ... )		log_manager()->_log( __FILENAME__, __LINE__, bryllite::log_level::warning, fmt, ##__VA_ARGS__ )
#define		ilog( fmt, ... )		log_manager()->_log( __FILENAME__, __LINE__, bryllite::log_level::info, fmt, ##__VA_ARGS__ )
#define		log( fmt, ... )			log_manager()->_log( __FILENAME__, __LINE__, bryllite::log_level::info, fmt, ##__VA_ARGS__ )
#define		dlog( fmt, ... )		log_manager()->_log( __FILENAME__, __LINE__, bryllite::log_level::debug, fmt, ##__VA_ARGS__ )
#define		ulog( fmt, ... )		log_manager()->_log( __FILENAME__, __LINE__, bryllite::log_level::user, fmt, ##__VA_ARGS__ )

#define		nlog( fmt, ... )		log_manager()->_native( fmt, ##__VA_ARGS__ )

// initialize log
bool log_initialize(const char* filename = nullptr, bryllite::log_attr::type logAttr = bryllite::log_attr::none, bryllite::log_level::type logFilter = bryllite::log_level::all);


#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_LOG_HPP_
