/*
 * intrusive_list.hpp
 *
 *  Created on: 22.12.2014
 *      Author: Michael
 */

#ifndef INTRUSIVE_LIST_HPP_
#define INTRUSIVE_LIST_HPP_

#include <microlib/concurrency.hpp>

namespace util {

	template< typename T, typename ConcurrencyTrait = NoConcurrency >
	struct intrusive_stack :
			public ConcurrencyTrait
	{

		intrusive_stack()
			: top_(nullptr)
		{}

		void push( T* ptr ) {
			ConcurrencyTrait::protect();
			intrusive_stack_set_next( ptr, top_ );
			top_ = ptr;
			ConcurrencyTrait::unprotect();
		}

		T* pop() {
			ConcurrencyTrait::protect();
			T* ret = top_;
			top_ = intrusive_stack_get_next( top_ );
			ConcurrencyTrait::unprotect();
			return ret;
		}

		bool empty() const {
			return top_ == nullptr;
		}

		T* top_;

	};

}


#endif /* INTRUSIVE_LIST_HPP_ */
