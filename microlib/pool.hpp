//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_POOL_HPP__
#define MICROLIB_POOL_HPP__

#include <microlib/concurrency.hpp>
#include <microlib/util.hpp>
#include <utility>
#include <cstddef>
#include <new>

namespace ulib {

	namespace detail {

		template< typename Type >
		struct pool_element_type;

		template< typename Type >
		struct trampoline {
			// this is effectively an inverted virtual table...
			void* pool;
			void(*func)(pool_element_type<Type>*, void*);
		};


		template< typename Type >
		struct pool_element_type
		{
			template< typename... Args >
			void construct(Args&&... args) {
				new (&storage_) Type(std::forward<Args>(args)...);
			}

			void destroy() {
				reinterpret_cast<Type*>(&storage_)->~Type();
			}

			void set_next_free(pool_element_type* next_free) {
				*reinterpret_cast<pool_element_type**>(&storage_) = next_free;
			}

			pool_element_type* get_next_free()
			{
				return *reinterpret_cast<pool_element_type**>(&storage_);
			}

			Type* get_ptr()
			{
				return reinterpret_cast<Type*>(&storage_);
			}

			const Type* get_ptr() const
			{
				return reinterpret_cast<const Type*>(&storage_);
			}

			using storage_type = typename std::aligned_storage<max(sizeof(Type), sizeof(pool_element_type*)), max(std::alignment_of<Type>::value, std::alignment_of<pool_element_type*>::value)>::type;
			storage_type storage_;

			trampoline<Type>* trampoline_;

			void free()
			{
				trampoline_->func(this, trampoline_->pool);
			}
		};

	}

	template< typename Type >
	struct pool_ptr {
		using element_type = detail::pool_element_type<Type>;
		using pointer_type = detail::pool_element_type<Type>*;

		pool_ptr()
			: the_element_(nullptr)
		{}

		pool_ptr(const pool_ptr&) = delete;
		pool_ptr operator=(const pool_ptr&) = delete;

		pool_ptr(pointer_type the_element)
			: the_element_(the_element)
		{}

		pool_ptr(pool_ptr&& other)
			: the_element_(nullptr)
		{
			std::swap(the_element_, other.the_element_);
		}

		void clear() {
			if (the_element_) {
				auto* buff = the_element_;
				the_element_ = nullptr;
				buff->free();
			}
		}

		// Only call this if you're absolutely sure that the object being pointed to
		// is handled by a pool and is casted to the exact pool type.
		static pool_ptr from_type_ptr(Type* ptr)
		{
			if (!ptr) {
				return pool_ptr();
			} else {
				return pool_ptr(reinterpret_cast<pointer_type>(ptr));
			}
		}

		//
		// use from_type_ptr to reverse this

		Type* release_type()
		{
			auto* result = get_payload();
			the_element_ = nullptr;
			return result;
		}

		pointer_type get()
		{
			return the_element_;
		}

		pointer_type release()
		{
			auto result = the_element_;
			the_element_ = nullptr;
			return result;
		}

		~pool_ptr() {
			clear();
		}

		pool_ptr& operator=(pool_ptr&& other)
		{
			clear();
			std::swap(the_element_, other.the_element_);
			return *this;
		}

		Type* get_payload()
		{
			return the_element_->get_ptr();
		}

		const Type* get_payload() const
		{
			return the_element_->get_ptr();
		}

		explicit operator bool() const {
			return the_element_ != nullptr;
		}

		Type* operator->()
		{
			return the_element_->get_ptr();
		}

		const Type* operator->() const
		{
			return the_element_->get_ptr();
		}

		Type& operator*() {
			return *get_payload();
		}

		const Type& operator*() const {
			return *get_payload();
		}

	private:
		pointer_type the_element_;
	};


	template< typename Type, size_t Size, /*size_t Padding = 0,*/ typename ConcurrencyTrait = NoConcurrency >
	struct pool : public ConcurrencyTrait {
		using element_type = detail::pool_element_type<Type>;
		using pointer_type = pool_ptr<Type>;

		pool()
		{
			trampoline_.func = free_func;
			trampoline_.pool = this;

			first_free_ = elements_;
			for (size_t i = 0; i<Size; ++i) {
				elements_[i].trampoline_ = &trampoline_;
				elements_[i].set_next_free(&elements_[i + 1]);
			}
			elements_[Size - 1].set_next_free(nullptr);
		}

		static void free_func(element_type* elem, void* arg)
		{
			reinterpret_cast<pool*>(arg)->release(elem);
		}

		template< typename... Args >
		pointer_type make(Args&&... args) {
			auto* dest = acquire();
			if (dest) {
				dest->template construct(std::forward<Args>(args)...);
				return pointer_type(dest);
			} else {
				return pointer_type();
			}
		}

	private:
#ifdef DEBUG_POOLS
		void check_integrity()
		{
			ConcurrencyTrait::protect();
			for (size_t i = 0; i<Size; ++i) {
				if (elements_[i].pool_ != this) {
					for (;;);
				}
			}

			auto* buff = first_free_;
			while (buff) {
				auto* next = buff->get_next_free();
				if (next) {
					if (next->pool_ != this) {
						for (;;);
					}
				}
				buff = next;
			}
			ConcurrencyTrait::unprotect();
		}
#endif

		element_type* acquire()
		{
#ifdef DEBUG_POOLS
			check_integrity();
#endif

			element_type* result = nullptr;
			ConcurrencyTrait::protect();
			if (first_free_) {
				result = first_free_;
				first_free_ = result->get_next_free();
			}
			ConcurrencyTrait::unprotect();
			return result;
		}

		void release(element_type* elem) {
#ifdef DEBUG_POOLS
			if (elem->pool_ != this) {
				for (;;);
			}
#endif
			elem->destroy();
			ConcurrencyTrait::protect();
			elem->set_next_free(first_free_);
			first_free_ = elem;
			ConcurrencyTrait::unprotect();
		}

	private:
		element_type elements_[Size];
		element_type* first_free_;
		detail::trampoline<Type> trampoline_;
	};


}


#endif
