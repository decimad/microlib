#ifndef STM_UTIL_SORTING_HPP__
#define STM_UTIL_SORTING_HPP__

#include <utility>
#include <algorithm>
#include <microlib/static_vector.hpp>

namespace util {

	//
	// We're using insertion sort for we expect small sets, it is simple, it is stable, it uses almost no stack, it works well on mostly presorted data
	// Use insert_binary only if comparison costs far outweigh swap costs!
	//
	// alternative to pursue might be a shell sort implementation

	namespace insertion_sort {

		template< typename Iterator, typename Compare >
		void insert_binary(Iterator begin, Iterator end, Compare compare)
		{
			if (begin != end) {
				std::rotate(std::upper_bound(begin, end - 1, *(end - 1)), end - 1, end);
			}
		}

		template< typename Iterator, typename Compare >
		void insert(Iterator begin, Iterator end, Compare compare)
		{
			if (begin != end) {
				for (auto temp = end - 1; temp != begin && !compare(*(temp - 1), *temp); --temp) {
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

	}

	namespace detail {

		// Simple empty-base-class optimizer.

		template< typename... Bases >
		struct ebo : public Bases... {
			ebo(const Bases&... args)
				: Bases(args)...
			{}
		};

	}

	template< typename T, size_t Size, typename Compare = std::less<T> >
	struct sorted_vector : private detail::ebo<Compare>
	{
	public:
		using container_type = static_vector<T, Size>;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;

	public:
		sorted_vector()
			: ebo(Compare())
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
				insertion_sort::insert_binary(data_.begin(), data_.end(), *static_cast<Compare*>(this));
				return true;
			} else {
				return false;
			}
		}

		void erase(const_iterator it)
		{
			data_.erase(it);
		}

		// Call this if you changed a property to the given item which changed the ordering invariant
		void restore_invariant(iterator it)
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

		T& operator[](size_t index)
		{
			return data_[index];
		}

		const T& operator[](size_t index) const
		{
			return data_[index];
		}

		size_t size() const
		{
			return data_.size();
		}

		size_t capacity() const
		{
			return data_.capacity();
		}

		void clear()
		{
			data_.clear();
		}

	private:
		static_vector<T, Size> data_;
	};

}

#endif