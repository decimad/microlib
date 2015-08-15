#ifndef MICROLIB_INTRUSIVE_RING_HPP_
#define MICROLIB_INTRUSIVE_RING_HPP_

#include <microlib/concurrency.hpp>

namespace ulib {

	template< typename Type, unsigned int Size, typename ConcurrencyTrait = NoConcurrency >
	struct intrusive_ringbuffer :
			public ConcurrencyTrait
	{

		intrusive_ringbuffer()
		{
			for(auto i = 0; i<Size-1; ++i) {
				intrusive_ring_set_next(elements+i, elements+i+1);
			}
			intrusive_ring_set_next(elements+Size-1, elements);

			committed_front_ = nullptr;
			idle_front_ = elements;
#ifdef DEBUG_RINGBUFFER

#endif
		}

		// Commits the current idle front
		void commit() {
			ConcurrencyTrait::protect();
			if(committed_front_ == nullptr) {
				committed_front_ = idle_front_;
			}

			idle_front_ = intrusive_ring_get_next(idle_front_);

			if(idle_front_ == committed_front_) {
				idle_front_ = nullptr;
			}
			ConcurrencyTrait::unprotect();
		}

		// Releases the current commit front
		void release() {
			ConcurrencyTrait::protect();
			if(idle_front_ == nullptr) {
				idle_front_ = committed_front_;
			}

			committed_front_ = intrusive_ring_get_next(committed_front_);

			if(committed_front_ == idle_front_) {
				committed_front_ = nullptr;
			}
			ConcurrencyTrait::unprotect();
		}

		bool full() {
			return idle_front_ == nullptr;
		}

		bool empty() {
			return committed_front_ == nullptr;
		}

		Type* idle_front()
		{
			return idle_front_;
		}

		Type* committed_front() {
			return committed_front_;
		}

		Type* begin() {
			return elements;
		}

		Type* end() {
			return elements+Size;
		}

		bool check_is_element(Type* ptr) const
		{
			for(size_t i=0;i<Size;++i) {
				if(elements+i == ptr) {
					return true;
				}
			}
			return false;
		}

		Type* idle_front_;
		Type* committed_front_;
		Type elements[Size];

	};

}





#endif /* INTRUSIVE_RING_HPP_ */
