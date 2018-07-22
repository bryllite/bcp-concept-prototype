/*
 @file: assert.hpp
 @desc: custom assert 
 @author: jade@bryllite.com
 @date: 2018-05-14
 @note: assert with reason
 */

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_ASSERT_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_ASSERT_HPP_


 // __FILENAME__ ( source code filename only )
#ifndef __FILENAME__ 
#ifdef _WIN32
#define	__FILENAME__	(strrchr(__FILE__, '\\' ) ? strrchr( __FILE__, '\\' ) + 1 : __FILE__ )
#else // _WIN32
#define	__FILENAME__	(strrchr(__FILE__, '/' ) ? strrchr( __FILE__, '/' ) + 1 : __FILE__ )
#endif // _WIN32
#endif // __FILENAME__


// namespace bryllite
namespace bryllite {

// assert handler
void _assert_handler(const char* expression, const char* filename, unsigned int line, const char* function, const char* fmt, ...);

}; // namespace bryllite

#ifdef NDEBUG
#define	_assert( e, fmt, ... )	((void)0)
#else //NDEBUG
#define	_assert( e, fmt, ... )	(void)( (!!(e)) || ( bryllite::_assert_handler( #e, (const char*)__FILENAME__, (unsigned int)__LINE__, (const char*)__func__, fmt, ##__VA_ARGS__ ), 0 )	)
#endif //NDEBUG

#endif // _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_ASSERT_HPP_

