//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_INTRUSIVE_LIST_HPP_
#define MICROLIB_INTRUSIVE_LIST_HPP_

#include <microlib/concurrency.hpp>

namespace ulib
{

    template <typename T, typename ConcurrencyTrait = NoConcurrency>
    struct intrusive_stack : public ConcurrencyTrait
    {

        intrusive_stack() : top_(nullptr)
        {
        }

        void push(T *ptr)
        {
            ConcurrencyTrait::protect();
            intrusive_stack_set_next(ptr, top_);
            top_ = ptr;
            ConcurrencyTrait::unprotect();
        }

        T *pop()
        {
            ConcurrencyTrait::protect();
            T *ret = top_;
            top_ = intrusive_stack_get_next(top_);
            ConcurrencyTrait::unprotect();
            return ret;
        }

        bool empty() const
        {
            return top_ == nullptr;
        }

        T *top_;
    };

} // namespace ulib

#endif /* INTRUSIVE_LIST_HPP_ */
