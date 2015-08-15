//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "stdafx.h"
#include <iostream>
#include <cassert>
#include <set>
#include <vector>
#include <algorithm>
#include "static_heap_test.hpp"
#include <microlib/static_heap.hpp>

void static_heap_test()
{
	ulib::static_heap<short, 32> int_heap;

	int_heap.emplace(0);
	int_heap.emplace(1);
	int_heap.emplace(2);
	int_heap.emplace(3);
	int_heap.emplace(4);
	int_heap.emplace(5);
	int_heap.emplace(6);
	int_heap.emplace(7);
	int_heap.emplace(8);
	int_heap.emplace(9);

	assert(int_heap.test_invariant());

	int_heap.replace(249);

	assert(int_heap.test_invariant());

	int_heap.replace(232);

	assert(int_heap.test_invariant());

	int_heap.replace(4);

	assert(int_heap.test_invariant());

	int_heap.replace(36);

	assert(int_heap.test_invariant());

	for (auto& i : int_heap)
	{
		std::cout << int(i) << ", ";
	}
	std::cout << "\n\n";

	assert(int_heap.test_invariant());
	*(int_heap.begin() + 6) = 0;
	assert(!int_heap.test_invariant());
	int_heap.restore(int_heap.begin() + 6);
	assert(int_heap.test_invariant());

	std::cout << "\n=====\n";
	std::cout << "size: " << int(int_heap.size()) << "\n\n";

	while (int_heap.size()) {
		auto& topref = int_heap.top_element();
		std::cout << "popping " << int(topref) << "\n";
		int_heap.pop();
	}

}