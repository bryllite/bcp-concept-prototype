#pragma once

#include <vector>
#include <map>


template< typename T>
class _vector
{
private:
	std::vector< T > _data;

public:
	_vector();

	// append value
	void add(T value);

	// get max value
	bool max_value(T& max);

	// get low value
	bool low_value(T& low);

	// count of value
	size_t count(T value);

	// size
	size_t size(void);

	// get value of count exceeds
	bool get_value_if_count_exceeds(T& value, size_t target_count);

	// clear
	void clear(void);
};



