//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_STATIC_DEQUE_HPP__
#define MICROLIB_STATIC_DEQUE_HPP__

#include <cassert>
#include <type_traits>
#include <new>
#include <utility>
#include "detail/calc.hpp"

namespace ulib {

	namespace impl {

		template< size_t Capacity >
		struct power_of_2_waste {
			static_assert( ulib::detail::is_power_of_2(Capacity), "Capacity must be a power of 2.");

			power_of_2_waste()
				: front_(0), back_(0)
			{}

			size_t pop_front_idx()
			{
				return (front_++) % Capacity;
			}

			size_t push_front_idx()
			{
				return (--front_)%Capacity;
			}

			size_t pop_back_idx()
			{
				return (--back_)%Capacity;
			}

			size_t push_back_idx()
			{
				return (back_++)%Capacity;
			}

			size_t size() const
			{
				return (back_-front_)%Capacity;
			}

			bool empty() const
			{
				return back_ == front_;
			}

			bool full() const
			{
				return size() == Capacity-1;
			}

			size_t front_index() const
			{
				return front_%Capacity;
			}

			size_t back_index() const
			{
				return (back_-1)%Capacity;
			}

			size_t front_;
			size_t back_;
		};

		template< size_t Capacity >
		struct power_of_2_nowaste {
			static_assert( ulib::detail::is_power_of_2(Capacity), "Capacity must be a power of 2.");

			power_of_2_nowaste()
				: front_(0), size_(0)
			{}

			size_t pop_front_idx()
			{
				--size_;
				return (front_++) % Capacity;
			}

			size_t push_front_idx()
			{
				++size_;
				return (--front_) % Capacity;
			}

			size_t pop_back_idx()
			{
				--size_;
				return (front_+size_) % Capacity;
			}

			size_t push_back_idx()
			{
				return (front_ + (size_++)) % Capacity;
			}

			size_t size() const
			{
				return size_;
			}

			bool empty() const
			{
				return size_ == 0;
			}

			bool full() const
			{
				return size_ == Capacity;
			}

			size_t front_index() const
			{
				return front_%Capacity;
			}

			size_t back_index() const
			{
				return (front_+size_-1)%Capacity;
			}

			size_t front_;
			size_t size_;
		};

		template< size_t Capacity >
		struct any_waste {
		};

		template< size_t Capacity >
		struct any_nowaste {
		};

	}

template< typename Type, size_t Capacity, template< size_t > class Impl = impl::power_of_2_waste >
class static_deque : private Impl< Capacity > {
    using impl = Impl<Capacity>;
public:
    static_deque()
    {}

    constexpr size_t capacity() const
    {
        return Capacity;
    }

    size_t size() const
    {
        return impl::size();
    }

    bool empty() const
    {
    	return impl::empty();
    }

    bool full() const
    {
    	return impl::full();
    }

    void push_front( Type val )
    {
        construct( impl::push_front_idx(), std::move( val ) );
    }

    void pop_front()
    {
        destroy( impl::pop_front_idx() );
    }

    void push_back( Type val )
    {
        construct( impl::push_back_idx(), std::move( val ) );
    }

    void pop_back()
    {
        destroy( impl::pop_back_idx() );
    }

    Type& front()
    {
        return dereference( impl::front_index() );
    }

    const Type& front() const
    {
        return dereference( impl::front_index() );
    }

    Type& back()
    {
        return dereference( impl::back_index() );
    }

    const Type& back() const
    {
        return dereference( impl::back_index() );
    }

    ~static_deque()
    {
        while( !impl::empty() ) {
            pop_front();
        }
    }

private:
    using element_storage_type = typename std::aligned_storage<sizeof( Type ), std::alignment_of<Type>::value>::type;

    void construct( size_t index, Type val )
    {
        new (&dereference(index)) Type( std::move( val ) );
    }

    void destroy( size_t index )
    {
        reinterpret_cast<Type*>(&data_[index])->~Type();
    }

    const Type& dereference( size_t index ) const
    {
        return *reinterpret_cast<const Type*>(&data_[index]);
    }

    Type& dereference( size_t index )
    {
        return *reinterpret_cast<Type*>(&data_[index]);
    }

    element_storage_type data_[Capacity];
};

}

#endif
