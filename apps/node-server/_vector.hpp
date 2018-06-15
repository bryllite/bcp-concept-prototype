#pragma once

#include <vector>


template< typename T>
class _vector
{
public:
	std::vector< T > _data;

public:
	_vector() {
	};

	// append value
	void add( T value )
	{
		_data.push_back( value );
	};

	// get max value in data
	bool max_value( T& value )
	{
		if( _data.size() == 0 ) return false;

		bool first = true;
		for( auto& v : _data )
		{
			if( first ) {
				first = false;
				value = v;
				continue;
			}

			if( v > value ) {
				value = v;
			}
		}

		return true;
	};

	// get min value in data
	bool min_value( T& value )
	{
		if( _data.size() == 0 ) return false;

		bool first = true;
		for( auto& v : _data )
		{
			if( first ) {
				first = false;
				value = v;
				continue;
			}

			if( v < value ) {
				value = v;
			}
		}

		return true;
	};

	// count of value
	size_t count( T value )
	{
		size_t cnt = 0;

		for( auto& v : _data ) {
			if( v == value ) cnt++;
		}

		return cnt;
	};

	// size of data
	size_t size( void ) {
		return _data.size();
	};

	// get value of count exceeds
	bool get_value_if_count_exceeds( T& value, size_t target_count )
	{
		for( auto& v : _data )
		{
			if( count( v ) > target_count )
			{
				value = v;
				return true;
			}
		}

		return false;
	};

	// clear container
	void clear( void ) {
		_data.clear();
	};
};
