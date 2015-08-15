//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "stdafx.h"
#include <iostream>
#include <chrono>
#include "sorted_static_vector_test.hpp"
#include <microlib/sorted_static_vector.hpp>

namespace {
	unsigned int a = 1140671485;
	unsigned int c = 12820163;
	unsigned int seed = 123541236;

	unsigned int myrand() {
		return seed = ((seed * a + c) & 0xFFFFFF);
	}
}

void sorted_static_vector_test()
{
	ulib::sorted_static_vector<int, 1024> heap;
	unsigned int pushes = 0;
	unsigned int pops = 0;

	std::cout << "Sorted Vector test:\n\n";

	auto begin = std::chrono::high_resolution_clock::now();

	for (unsigned int i = 0; i < 10000000; ++i) {
		for (unsigned int j = 0; j < (myrand() % (heap.capacity() - heap.size() + 1)); ++j)
		{
			heap.emplace_binary(myrand());
			++pushes;
		}

		for (unsigned int j = 0; j < (myrand() % (heap.size() + 1)); ++j) {
			if (myrand() % 2) {
				heap.pop_back(); //pop_max();
			} else {
				heap.pop_front(); //pop_min();
			}
			++pops;
		}
	}

	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "Pushes: " << pushes << "\n";
	std::cout << "Pops:   " << pops << "\n";
	std::cout << "Time:   " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "us\n";
	std::cout << "Ops/us: " << double(pushes + pops) / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "\n\n";

	std::cout << "Popping: " << heap.max_element() << "\n";
	heap.pop_back();

	/*
	while (heap.size()) {
		std::cout << "Popping: " << heap.max_element() << "\n";
		heap.pop_back();
	}
	*/

	std::cout << "\n";

	bool break_here = true;
}