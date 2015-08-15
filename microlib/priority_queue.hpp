#ifndef MICROLIB_PRIORITY_QUEUE_HPP__
#define MICROLIB_PRIORITY_QUEUE_HPP__

#include <microlib/static_vector.hpp>
#include <algorithm>

namespace ulib {
	
	// FIXME: Replace with with static_heap/static_interval_heap or sorted vector implementation

	template< typename Element, size_t Size >
	struct static_priority_queue {
	public:
		using container_type = static_vector< Element, Size >;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;

	public:
		template< typename... Args >
		bool insert(Args&&... args)
		{
			if (data_.emplace_back(std::forward<Args>(args)...)) {
				std::push_heap(data_.begin(), data_.end(), &priority_compare(const Element&, const Element&));
			} else {
				return false;
			}
		}

		void pop()
		{
			std::pop_heap(data_.begin(), data_.end());
		}

		Element& top() {
			return data_[0];
		}

		const Element& top() const {
			return data_[0];
		}

		iterator begin()
		{
			return data_.begin();
		}

		iterator end()
		{
			return data_.end();
		}

		const iterator begin() const
		{
			return data_.begin();
		}

		const_iterator end() const
		{
			return data_.end();
		}

		// try not to call this.
		void erase(const_iterator it)
		{
			data_.erase(it);
			std::make_heap(begin(), end());
		}

	private:
		static_vector< Element, Size > data_;
	};

	template< typename Element, size_t Size >
	struct static_priority_queue_wrap_around {
	public:
		using container_type = static_vector< Element, Size >;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;

	public:
		template< typename... Args >
		bool insert(Args&&... args)
		{
			if (data_.emplace_back(std::forward<Args>(args)...)) {
				std::push_heap(data_.begin(), data_.end(), &priority_compare(const Element&, const Element&));
			} else {
				return false;
			}
		}

		void pop()
		{
			std::pop_heap(data_.begin(), data_.end());
		}

		Element& top() {
			return data_[0];
		}

		const Element& top() const {
			return data_[0];
		}

		iterator begin()
		{
			return data_.begin();
		}

		iterator end()
		{
			return data_.end();
		}

		const iterator begin() const
		{
			return data_.begin();
		}

		const_iterator end() const
		{
			return data_.end();
		}

		// try not to call this.
		void erase(const_iterator it)
		{
			data_.erase(it);
			std::make_heap(begin(), end());
		}

	private:
		static_vector< Element, Size > data_;
	};

}

#endif
