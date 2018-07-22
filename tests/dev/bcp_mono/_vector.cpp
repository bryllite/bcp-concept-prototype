#pragma once

#include "_vector.hpp"
#include <bryllite-common.hpp>
#include <bryllite-core.hpp>

using block = bryllite::block;
using block_header = bryllite::block_header;
template class _vector<size_t>;
template class _vector<block_header>;
template class _vector<block>;

template<typename T>
_vector<T>::_vector()
{
}

template<typename T>
void _vector<T>::add(T value)
{
	_data.push_back(value);
}

template<typename T>
bool _vector<T>::max_value(T& max)
{
	if (_data.size() == 0) return false;

	bool first = true;
	for (auto& v : _data)
	{
		if (first) {
			first = false;
			max = v;
			continue;
		}

		if (v > max) {
			max = v;
		}
	}

	return true;
}

template<typename T>
bool _vector<T>::low_value(T& low)
{
	if (_data.size() == 0) return false;

	bool first = true;
	for (auto& v : _data)
	{
		if (first) {
			first = false;
			low = v;
			continue;
		}

		if (v < low) {
			low = v;
		}
	}

	return true;
};

template<typename T>
size_t _vector<T>::count(T value)
{
	size_t cnt = 0;

	for (auto& v : _data) {
		if (v == value) cnt++;
	}

	return cnt;
};

template<typename T>
size_t _vector<T>::size(void)
{
	return _data.size();
};

template<typename T>
bool _vector<T>::get_value_if_count_exceeds(T& value, size_t cnt)
{
#pragma _WARNING("exception on vs2015. 'vector iterator not incrementable'")
	//	for (auto& v : _data)	
	for( size_t i=0; i<_data.size(); i++ )
	{
		auto& v = _data[i];
		if (count(v) > cnt)
		{
			value = v;
			return true;
		}
	}

	return false;
};

template<typename T>
void _vector<T>::clear(void)
{
	_data.clear();
};



