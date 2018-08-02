/*
@file: utils.hpp
@desc: bryllite utils
@author: jade@bryllite.com
@date: 2018-05-14
@note: utils
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_UTILS_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_UTILS_HPP_

// #pragma 와 함께 써야한다.
// e.g. #pragma _TODO("언제까지 이 일을 해야함")
#define	__MAKESTRING_HELPER(x)	#x
#define	__MAKESTRING(x)			__MAKESTRING_HELPER(x)
#define	__MESSAGE(msg)			message( __FILE__ "(" __MAKESTRING( __LINE__ ) "): " msg ) 
#define	_WARNING(msg)			__MESSAGE( "[WARNING]: " #msg )
#define	_MESSAGE(msg)			__MESSAGE( "[MESSAGE]: " #msg )
#define	_TODO(msg)				__MESSAGE( "[TODO]: " #msg )
//#define	_ERROR(msg)				__MESSAGE( "[ERROR]: " #msg )	// use #error message

// namespace bryllite
namespace bryllite {

	// sleep for milliseconds
	void sleep_for(unsigned int ms);

	void sleep(unsigned int ms = 0);

	// switch to another thread
	void yield(void);

	// unused
	template <typename T >
	void unused(T&&) {
		// for remove unused variable warnings.
	}

	// string format
	std::string _format(const char* fmt, std::va_list& args);

	// string format
	std::string format(const char* fmt, ...);

	// string parser
	std::vector< std::string > parse(const std::string str, const std::string delimiter, bool remove_empty = false);

	// string replace
	std::string replace(std::string str, std::string before, std::string after);

	std::string replace(std::string str, const char* before, const char* after);

	int rand(void);

	// byte array -> hex string
	std::string toHexCode(unsigned char* data, size_t data_len);

	// hex string -> byte array
	void toBytes(std::string str, unsigned char* data, size_t data_len);

	std::string byte_to_hex( uint8_t ch );
	std::string bytes_to_hex( std::vector<uint8_t> vch );
	std::string bytes_to_hex( unsigned char* data, size_t len );
	std::vector<uint8_t> hex_to_bytes( std::string hex );

#define		NAMESPACE_BEGIN(x)		namespace x {
#define		NAMESPACE_END(x)		}


}; // namespace bryllite

#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_UTILS_HPP_