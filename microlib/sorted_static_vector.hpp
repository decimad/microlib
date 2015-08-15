//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_SORTING_HPP__
#define MICROLIB_SORTING_HPP__

#include <utility>
#include <algorithm>
#include <microlib/static_vector.hpp>
#include <microlib/util.hpp>

namespace ulib {

	//
	// We're using insertion sort for we expect small sets, it is simple, it is stable, it uses almost no stack, it works well on mostly presorted data
	// Use insert_binary only if comparison costs far outweigh swap costs!
	//
	// alternative to pursue might be a shell sort implementation

	namespace insertion_sort {

		template< typename Iterator, typename Compare >
		void insert_binary_back(Iterator begin, Iterator end, Compare&& compare)
		{		
			std::rotate(std::upper_bound(begin, end - 1, *(end - 1), std::forward<Compare>(compare)), end - 1, end);
		}

		template< typename Iterator, typename Compare >
		void insert_binary_front(Iterator begin, Iterator end, Compare&& compare)
		{
			std::rotate(begin, std::upper_bound(begin + 1, end, *begin, std::forward<Compare>(compare)), end);
		}


		template< typename Iterator, typename Compare >
		void insert(Iterator begin, Iterator end, Compare&& compare)
		{
			if (begin != end) {
				for (auto temp = end - 1; temp != begin && !compare(*(temp - 1), *temp, std::forward<Compare>(compare)); --temp) {
					std::swap(*(temp - 1), *temp);
				}
			}
		}

		template< typename Iterator, typename Compare >
		void insertion_sort(Iterator begin, Iterator end, Compare compare)
		{
			for (auto it = begin; it != end; ++it) {
				insert(begin, it, compare);
			}
		}


		template< typename Iterator >
		void insertion_sort(Iterator begin, Iterator end)
		{
			insertion_sort(begin, end, std::less<>());
		}

		template< typename Iterator, typename Compare >
		void insertion_sort_binary(Iterator begin, Iterator end, Compare compare)
		{
			for (auto it = begin; it != end; ++it) {
				insert_binary(begin, it, compare);
			}
		}

		template< typename Iterator>
		void insertion_sort_binary(Iterator begin, Iterator end)
		{
			insertion_sort_binary(begin, end, std::less<>());
		}

		template< typename Iterator, typename Compare >
		void restore_invariant(Iterator begin, Iterator end, Iterator elem, Compare&& compare)
		{
			while (elem != begin && !compare(*(elem - 1), *elem)) {
				std::swap(*(elem - 1), *elem);
				--elem;
			}

			while (elem + 1 != end && compare(*(elem + 1), *elem)) {
				std::swap(*(elem + 1), *elem);
				++elem;
			}
		}

		template< typename Iterator, typename Compare >
		void restore_invariant_front(Iterator begin, Iterator end, Compare&& compare)
		{
			insert_binary_front(begin, end, std::forward<Compare>(compare));
		}

		template< typename Iterator, typename Compare >
		void restore_invariant_back(Iterator begin, Iterator end, Compare&& compare)
		{
			insert_binary_back(begin, end, std::forward<Compare>(compare));
		}

	}

	template< typename T, size_t Size, typename Compare = std::less<T> >
	struct sorted_static_vector : private detail::ebo<Compare>
	{
	public:
		using container_type = static_vector<T, Size>;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;
		using size_type = typename container_type::size_type;

	public:
		template< typename... Args >
		sorted_static_vector( Args&&... args )
			: detail::ebo<Compare>(Compare(std::forward<Args>(args)...))
		{}

		template<typename... Args>
		bool emplace(Args&&... args)
		{
			if (data_.emplace_back(std::forward<Args>(args)...)) {
				insertion_sort::insert(data_.begin(), data_.end(), *static_cast<Compare*>(this));
				return true;
			} else {
				return false;
			}
		}

		template<typename... Args>
		bool emplace_binary(Args&&... args)
		{
			if (data_.emplace_back(std::forward<Args>(args)...)) {
				insertion_sort::insert_binary_back(data_.begin(), data_.end(), *static_cast<Compare*>(this));
				return true;
			} else {
				return false;
			}
		}

		void erase(iterator it)
		{
			data_.erase(it);
		}

		template< typename ValType >
		void replace_min(ValType&& val)
		{
			data_[0] = std::forward<ValType>(val);
			insertion_sort::restore_invariant_front(begin(), end(), *static_cast<Compare*>(this));
		}

		template< typename ValType >
		void replace_max(ValType&& val)
		{
			data_.back() = std::forward<ValType>(val);
			insertion_sort::restore_invariant_front(begin(), end(), *static_cast<Compare*>(this));
		}

		T& min_element()
		{
			return data_[0];
		}

		const T& min_element() const
		{
			return data_[0];
		}

		T& max_element()
		{
			return data_.back();
		}

		const T& max_element() const
		{
			return data_.back();
		}

		// Call this if you changed a property to the given item which changed the ordering invariant
		void restore(iterator it)
		{
			insertion_sort::restore_invariant(begin(), end(), it, *static_cast<Compare*>(this));
		}

		iterator begin() {
			return data_.begin();
		}

		iterator end() {
			return data_.end();
		}

		const_iterator begin() const {
			return data_.begin();
		}

		const_iterator end() const {
			return data_.end();
		}

		T& operator[](size_type index)
		{
			return data_[index];
		}

		const T& operator[](size_type index) const
		{
			return data_[index];
		}

		size_type size() const
		{
			return data_.size();
		}

		size_type capacity() const
		{
			return data_.capacity();
		}

		void clear()
		{
			data_.clear();
		}

		void pop_back()
		{
			data_.pop_back();
		}

		void pop_front()
		{
			data_.pop_front();
		}

	private:
		static_vector<T, Size> data_;
	};

}

#endif
