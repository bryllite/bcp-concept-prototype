/*
@file: colors.hpp
@desc: ansi color header
@author: jade@bryllite.com
@date: 2018-05-16
@note: ansi color code definition
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_COLORS_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_COLORS_HPP_


namespace bryllite {

struct colors 
{
	// console color
	enum type {
		black = 0,
		white, gray,

		red, blue, green, yellow, magenta, cyan,
		light_red, light_blue, light_green, light_yellow, light_magenta, light_cyan,

		_default = gray,
	};

	// set color on windows or return ansi code for color
	// usage: std::cout << colors::set( colors::green ) << "color: green" << std::endl ;
	static const char* set( colors::type cr );

#ifdef _WIN32
	static unsigned short val( colors::type cr ) ;
#else // _WIN32
	static const char* val( colors::type cr ) ;
#endif // _WIN32

};


}; // namespace bryllite


#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_COLORS_HPP_
