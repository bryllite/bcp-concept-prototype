/*
@file: enum.hpp
@desc: enum class bitmask operation template
@author: jade@bryllite.com
@date: 2018-05-14
@note: enum class bitmask operation
*/

#if !defined( _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_ENUM_HPP_ )
#define _BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_ENUM_HPP_

#include <type_traits>

template<typename Enum>
struct EnableBitMaskOperators
{
	static const bool enable = false;
};

#define enable_enum_bitmask_operators(x)  \
template<>                           \
struct EnableBitMaskOperators<x>     \
{                                    \
    static const bool enable = true; \
};

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator &( Enum lhs, Enum rhs )
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<Enum> ( static_cast<underlying>( lhs ) & static_cast<underlying>( rhs ) );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator ^( Enum lhs, Enum rhs )
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<Enum> ( static_cast<underlying>( lhs ) ^ static_cast<underlying>( rhs ) );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator ~( Enum rhs )
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<Enum> ( ~static_cast<underlying>( rhs ) );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator |( Enum lhs, Enum rhs )
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<Enum> ( static_cast<underlying>( lhs ) | static_cast<underlying>( rhs ) );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type&
operator &=( Enum& lhs, Enum rhs )
{
	using underlying = typename std::underlying_type<Enum>::type;
	lhs = static_cast<Enum> ( static_cast<underlying>( lhs ) & static_cast<underlying>( rhs ) );
	return lhs;
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type&
operator ^=( Enum& lhs, Enum rhs )
{
	using underlying = typename std::underlying_type<Enum>::type;
	lhs = static_cast<Enum> ( static_cast<underlying>( lhs ) ^ static_cast<underlying>( rhs ) );
	return lhs;
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum&>::type
operator |=( Enum& lhs, Enum rhs )
{
	using underlying = typename std::underlying_type<Enum>::type;
	lhs = static_cast<Enum> ( static_cast<underlying>( lhs ) | static_cast<underlying>( rhs ) );
	return lhs;
}

#endif //_BRYLLITE_PLATFORM_BRYLLITE_COMMON_LIB_ENUM_HPP_