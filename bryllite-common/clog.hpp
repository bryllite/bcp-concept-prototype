#pragma once

// colors for log
enum class Color
{
	black = 0,
	white, gray,
	red, green, blue, yellow, magenta, cyan,
	RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN,
	default = gray
};

// get color value
#ifdef _WIN32
unsigned short color_val( Color cr );
#else
const char* color_val( Color cr );
#endif

// source code position: (main.cpp:100)
#ifndef __SRC__
#define __SRC__		bryllite::CSourceCode(__FILENAME__, __LINE__)
#endif

// current time code: ([19:36:14]: )
#ifndef __TIMECODE__
#define __TIMECODE__	bryllite::CTimeCode()
#endif

// namespace bryllite
namespace bryllite 
{
	// log end
	class CLogEnd
	{
	public:
		std::string CRLF;
		CLogEnd();
	};

	// time code
	class CTimeCode
	{
	public:
		std::string TIMECODE;
		CTimeCode();
	};

	// source code
	class CSourceCode
	{
	public:
		std::string SOURCECODE;
		CSourceCode( const char* file, int line );
	};

	// log class
	class CLog 
	{
	public:

		// log attribute
		enum class Attribute
		{
			none = 0x0000,
			time = 0x0001,			// show time
			source = 0x0002,		// show source code info
			callstack = 0x0004,		// show callstacks
			file = 0x0008,			// write log to file

			all = 0xffff
		};

		// log level
		enum class Level
		{
			off = 0x0000,
			fatal = 0x0001,			// fatal error: system will be shutdown ( RED )
			error = 0x0002,			// error: system will not be shutdown, but fix it ( LIGHT RED )
			warning = 0x0004,		// warning ( YELLOW )
			notice = 0x0008,		// notice ( LIGHT YELLOW )
			verbose = 0x0010,		// verbose ( GRAY )
			info = 0x0020,			// information ( white )
			debug = 0x0040,			// debug ( CYAN )
			trace = 0x0080,			// trace ( BLUE )
			user = 0x0100,			// user ( MAGENTA )

			all = 0xffff
		};

	protected:
		static constexpr const char* DEFAULT_LOG_FILE = "logs/CLog-%y-%m-%d.log";

		lockable _lock;

		// log file ( with path )
		std::string _logFile;

		// prev log file 
		std::string _lastLogFile;

		// outfile stream
		std::fstream _file;

		// log attribute
		Attribute _logAttr;

		// log filter
		Level _logFilter;

	public:
		CLog();
		CLog( std::string logFile );
		~CLog();

		void initialize( std::string logFile, Attribute logAttr, Level logFilter );

		void logFile( std::string logFile );
		std::string logFile( void );

		void logAttr( Attribute attr );
		Attribute logAttr( void );

		void logFilter( Level filter );
		Level logFilter( void );

		CLog& instance( void );

		// write Line log
		CLog& writeLine( std::string msg );
		CLog& writeLine( Level logLevel, std::string msg );
		CLog& writeLine( Level logLevel, const char* file, int line, std::string msg );

		CLog& writeLine( const char* fmt, ... );
		CLog& writeLine( Level logLevel, const char* fmt, ... );
		CLog& writeLine( Level logLevel, const char* fileName, int line, const char* fmt, ... );
		CLog& writeLine( Level logLevel, const CSourceCode& src, const char* fmt, ... );

	public:
		// operator <<
		CLog& operator<< ( Color cr );
		CLog& operator<< ( const CLogEnd& end );
		CLog& operator<< ( const CTimeCode& timeCode );
		CLog& operator<< ( const CSourceCode& sourceCode );
		CLog& operator<< ( const Level logLevel );
		CLog& operator<< ( std::string val );
		CLog& operator<< ( const char* val );
		CLog& operator<< ( bool val );
		CLog& operator<< ( char val );
		CLog& operator<< ( unsigned char val );
		CLog& operator<< ( short val );
		CLog& operator<< ( unsigned short val );
		CLog& operator<< ( int val );
		CLog& operator<< ( unsigned int val );
		CLog& operator<< ( long val );
		CLog& operator<< ( unsigned long val );
		CLog& operator<< ( long long val );
		CLog& operator<< ( unsigned long long val );
		CLog& operator<< ( float val );
		CLog& operator<< ( double val );
		CLog& operator<< ( long double val );

	protected:

		// create log file directory if not exists
		void createDirectory( std::string logFile );

		// set text color
		void setTextColor( Color cr );

		// rotate log file
		bool rotateLogFile( void );

		// filter log level
		bool filter( Level level );

		// attribute have?
		bool hasAttr( Attribute attr );

		void printTimeCode( void );
		void printLogLevel( Level logLevel );
		void printSourceCode( const char* file, int line );
	};

	// carriage return & line feed
	extern CLogEnd endl;

	// CLog instance
	extern CLog clog;

}; // namespace bryllite


// CLog macros
//#define fatal( fmt, ... )		bryllite::clog.write( bryllite::CLog::Level::fatal, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__ ) 
//#define error( fmt, ... )		bryllite::clog.write( bryllite::CLog::Level::error, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__ ) 
//#define warn( fmt, ... )		bryllite::clog.write( bryllite::CLog::Level::warning, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__ ) 
//#define notice( fmt, ... )		bryllite::clog.write( bryllite::CLog::Level::notice, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__ ) 
//#define info( fmt, ... )		bryllite::clog.write( bryllite::CLog::Level::info, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__ ) 
//#define debug( fmt, ... )		bryllite::clog.write( bryllite::CLog::Level::debug, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__ ) 
//#define trace( fmt, ... )		bryllite::clog.write( bryllite::CLog::Level::trace, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__ ) 
//#define user( fmt, ... )		bryllite::clog.write( bryllite::CLog::Level::user, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__ ) 


#define fatal( fmt, ... )		bryllite::clog.writeLine( bryllite::CLog::Level::fatal, __SRC__, fmt, ##__VA_ARGS__ )
#define elog( fmt, ... )		bryllite::clog.writeLine( bryllite::CLog::Level::error, __SRC__, fmt, ##__VA_ARGS__ ) 
#define wlog( fmt, ... )		bryllite::clog.writeLine( bryllite::CLog::Level::warning, __SRC__, fmt, ##__VA_ARGS__ ) 
#define nlog( fmt, ... )		bryllite::clog.writeLine( bryllite::CLog::Level::notice, __SRC__, fmt, ##__VA_ARGS__ ) 
#define ilog( fmt, ... )		bryllite::clog.writeLine( bryllite::CLog::Level::info, __SRC__, fmt, ##__VA_ARGS__ ) 
#define dlog( fmt, ... )		bryllite::clog.writeLine( bryllite::CLog::Level::debug, __SRC__, fmt, ##__VA_ARGS__ ) 
#define tlog( fmt, ... )		bryllite::clog.writeLine( bryllite::CLog::Level::trace, __SRC__, fmt, ##__VA_ARGS__ ) 
#define ulog( fmt, ... )		bryllite::clog.writeLine( bryllite::CLog::Level::user, __SRC__, fmt, ##__VA_ARGS__ ) 

#define log( fmt, ... )			bryllite::clog.writeLine( bryllite::CLog::Level::verbose, __SRC__, fmt, ##__VA_ARGS__ ) 



