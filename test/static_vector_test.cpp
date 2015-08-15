//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "stdafx.h"
#include <cassert>
#include <microlib/static_vector.hpp>

class some_class {
public:
	some_class(size_t id)
		: id_(id)
	{
		++constructed_;
	}

	~some_class()
	{
		--constructed_;
	}

	size_t id_;
	static size_t constructed_;
};

size_t some_class::constructed_ = 0;

void static_vector_test()
{
	constexpr auto s1 = sizeof(ulib::static_vector<int, 32, true>);
	constexpr auto s2 = sizeof(ulib::static_vector<char, 32, true>);
	
	{
		// zero based vector
		ulib::static_vector<some_class, 32> zero_vec;
		assert(zero_vec.capacity() == 32);

		for (unsigned int i = 0; i < 32; ++i) {
			assert(zero_vec.emplace_back(i));
		}
		assert(zero_vec[0].id_ == 0);
		assert(some_class::constructed_ == 32);
		zero_vec.clear();
		assert(some_class::constructed_ == 0);
	}

	{
		// one based vector
		ulib::static_vector<some_class, 32, true> one_vec;
		assert(one_vec.capacity() == 31);

		for (unsigned int i = 0; i < 31; ++i) {
			assert(one_vec.emplace_back(i));
		}
		assert(one_vec[1].id_ == 0);
		assert(some_class::constructed_ == 31);

		one_vec.clear();
		assert(some_class::constructed_ == 0);

	}
}