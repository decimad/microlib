//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "static_interval_heap_test.hpp"
#include "stdafx.h"
#include <chrono>
#include <iostream>
#include <microlib/static_interval_heap.hpp>


namespace
{
    unsigned int a = 1140671485;
    unsigned int c = 12820163;
    unsigned int seed = 123541236;

    unsigned int myrand()
    {
        return seed = ((seed * a + c) & 0xFFFFFF);
    }
} // namespace

void static_interval_heap_test()
{
    ulib::static_inverval_heap<int, 1024> heap;
    unsigned int pushes = 0;
    unsigned int pops = 0;

    std::cout << "Interval Heap test:\n\n";

    auto begin = std::chrono::high_resolution_clock::now();

    for (unsigned int i = 0; i < 10000000; ++i)
    {
        for (unsigned int j = 0; j < (myrand() % (heap.capacity() - heap.size() + 1)); ++j)
        {
            heap.push(myrand());
            ++pushes;
        }

        for (unsigned int j = 0; j < (myrand() % (heap.size() + 1)); ++j)
        {
            if (myrand() % 2)
            {
                heap.pop_max();
            }
            else
            {
                heap.pop_min();
            }
            ++pops;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Pushes: " << pushes << "\n";
    std::cout << "Pops:   " << pops << "\n";
    std::cout << "Time:   " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "us\n";
    std::cout << "Ops/us: " << double(pushes + pops) / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "\n\n";

    /*


    heap.push(250);
    heap.push(251);
    heap.push(252);
    heap.push(253);
    heap.push(254);
    heap.push(255);
    heap.push(0);
    heap.push(252);
    heap.push(1);
    heap.push(2);

    *(heap.begin()+7) = 3214124;
    heap.fix(heap.begin()+7);

    heap.replace_min(312451);
    heap.replace_min(423512);
    heap.replace_min(215163);
    */

    /*
    for (unsigned int i = 0; i < 10; ++i) {
        std::cout << "Popping: " << heap.max_element() << "\n";
        heap.pop_max();
    }
    */

    std::cout << "Popping: " << heap.max_element() << "\n";
    heap.pop_max();

    /*
    while (heap.size()) {
        std::cout << "Popping: " << heap.max_element() << "\n";
        heap.pop_max();
    }
    */

    std::cout << "\n";

    bool break_here = true;
}