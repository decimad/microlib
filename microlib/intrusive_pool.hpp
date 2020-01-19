//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_INTRUSIVE_POOL_HPP_
#define MICROLIB_INTRUSIVE_POOL_HPP_

#include <cstddef>
#include <microlib/concurrency.hpp>
#include <new>
#include <utility>


// FIXME, get rid of
#include <microlib/util.hpp>
//#define DEBUG_POOLS

namespace ulib
{

    /*
     * We could use smart pointers here,
     * but almost no lib makes use of them,
     * so we'd be unpacking all the time
     *
     */

    /*
     * Note also that this is basically only a static store using an intrusive stack as a freelist
     *
     */

    template <typename Type, unsigned int Size, typename ConcurrencyTrait = NoConcurrency>
    struct intrusive_pool : public ConcurrencyTrait
    {
        intrusive_pool()
        {
            for (unsigned int i = 0; i < Size - 1; ++i)
            {
                intrusive_pool_set_next_free(elements + i, elements + i + 1);
            }
            intrusive_pool_set_next_free(elements + Size - 1, nullptr);
            first_free = elements;

#ifdef DEBUG_POOLS
            free_count = Size;
            check_integrity();
#endif
        }

        void release(Type *ptr)
        {
            ConcurrencyTrait::protect();
            intrusive_pool_set_next_free(ptr, first_free);
            first_free = ptr;
#ifdef DEBUG_POOLS
            ++free_count;
            check_integrity();
#endif
            ConcurrencyTrait::unprotect();
        }

        Type *acquire()
        {
            if (first_free)
            {
                ConcurrencyTrait::protect();
                auto *ret = first_free;
                first_free = intrusive_pool_get_next_free(first_free);

#ifdef DEBUG_POOLS
                intrusive_pool_set_next_free(ret, nullptr);
                --free_count;
                check_integrity();
#endif
                ConcurrencyTrait::unprotect();
                return ret;
            }
            else
            {
                return nullptr;
            }
        }

        Type *begin()
        {
            return elements;
        }

        Type *end()
        {
            return elements + Size;
        }

#ifdef DEBUG_POOLS
        bool check_is_element(Type *ptr)
        {
            for (size_t i = 0; i < Size; ++i)
            {
                if (elements + i == ptr)
                {
                    return true;
                }
            }

            return false;
        }

        void check_integrity()
        {
            unsigned int count = 0;
            Type *curr = first_free;
            while (curr)
            {
                ++count;
                if (curr - elements >= Size)
                {
                    for (;;)
                        ;
                }

                curr = intrusive_pool_get_next_free(curr);
            }

            if (count != free_count)
            {
                for (;;)
                    ;
            }
        }
#endif

        Type *first_free;
        Type elements[Size];

#ifdef DEBUG_POOLS
        unsigned int free_count;
#endif
    };

} // namespace ulib

#endif /* INTRUSIVE_POOL_HPP_ */
