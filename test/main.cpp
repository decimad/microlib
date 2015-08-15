//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "stdafx.h"
/*
template< typename Iterator >
auto sum( Iterator begin, Iterator end )
{
	std::decay_t<decltype(*begin)> value = 0;
	for (auto it = begin; it != end;++it) {
		value += *it;
	}

	return value;
}

template<typename T, typename S = std::enable_if_t<std::is_unsigned<T>::value>>
struct overflow_compare
{
	constexpr overflow_compare() {}

	constexpr bool operator()(T a, T b) const {
		return T(b - a) <= (std::numeric_limits<T>::max() / 2);
	}
};
*/

#include "static_vector_test.hpp"
#include "static_heap_test.hpp"
#include "static_interval_heap_test.hpp"
#include "sorted_static_vector_test.hpp"


int main()
{
	static_vector_test();
	static_heap_test();
	static_interval_heap_test();
	sorted_static_vector_test();
}

