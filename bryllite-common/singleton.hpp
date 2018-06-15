/*
@file: singleton.hpp
@desc: singleton pattern template
@author: jade@bryllite.com
@date: 2018-05-14
@note: singleton pattern
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_SINGLETON_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_SINGLETON_HPP_

#include <cstdlib>

// namespace bryllite
namespace bryllite {


// singleton template
template< typename T >
class singleton 
{
public:
	static T* instance( void ) ;
	static void release( void ) ;
	static bool allocated( void ) ;

private:
	static T* m_pInstance ;
};

template< typename T > 
T* singleton<T>::m_pInstance = nullptr ;

template< typename T >
T* singleton<T>::instance( void )
{
	if ( m_pInstance == nullptr ) {
		m_pInstance = new T() ;

		atexit( release ) ;
	}

	return m_pInstance ;
}

template< typename T >
void singleton<T>::release( void )
{
	if ( nullptr != m_pInstance ) delete m_pInstance ;
	m_pInstance = nullptr ;
}

template< typename T >
bool singleton<T>::allocated( void )
{
	return ( nullptr != m_pInstance ) ;
}

}; // namespace bryllite

#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_SINGLETON_HPP_
