#ifndef STM_UTIL_STATIC_HEAP_HPP__
#define STM_UTIL_STATIC_HEAP_HPP__

#include <microlib/static_vector.hpp>
#include <microlib/utilshared.hpp>

namespace util {

	//
	// Since std::heap stuff does not support random erasing, which is a must have, we have to implement
	// sift operations, thus we can implement the whole stuff...
	//
	// note: largely untested...
	//

	template< typename T, size_t Size, typename Compare = std::less<T> >
	struct static_heap : public detail::ebo<Compare>
	{
	public:
		using container_type = static_vector< T, Size >;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;

		static_heap(Compare compare = Compare())
			: detail::ebo<Compare>(std::move(compare))
		{}

		bool push(T val) {
			if (data_.emplace_back(std::move(val))) {
				sift_up(data_.size() - 1);
				return true;
			} else {
				return false;
			}
		}

		template<typename... Args>
		bool emplace(Args&&... args)
		{
			if (data_.emplace_back(std::forward<Args>(args)...)) {
				sift_up(data_.size() - 1);
				return true;
			} else {
				return false;
			}
		}

		void replace(T val) {
			std::swap(data_[rooti()], val);
			sift_down(rooti());
		}

		void replaced() {
			sift_down(rooti());
		}

		void pop() {
			if (data_.size() > 0) {
				if (data_.size() > 1) {
					std::swap(data_[0], data_[data_.size() - 1]);
					data_.pop_back();
					sift_down(0);
				} else {
					data_.pop_back();
				}
			}
		}

		T& root() {
			return data_[rooti()];
		}

		T& root() const {
			return data_[rooti()];
		}

		iterator begin() {
			return data_.begin() + rooti();
		}

		iterator end() {
			return data_.end();
		}

		const_iterator begin() const {
			return data_.begin() + rooti();
		}

		const_iterator end() const {
			return data_.end();
		}

		void fix(const_iterator it) {
			sift_fix(it - begin());
		}

		void fix_up(const_iterator it) {
			sift_up(it - begin());
		}

		void fix_down(const_iterator it) {
			sift_down(it - begin());
		}

		size_t size() const {
			return data_.size() - rooti();
		}

		void erase(const_iterator it)
		{
			auto idx = it - begin();

			if (idx < data_.size() - 1) {
				std::swap(data_[data_.size() - 1], data_[idx]);
				data_.pop_back();
				sift_down(idx);
			}
		}

	private:
		void sift_up(size_t i) {
			while (i > rooti() && !compare(data_[parent(i)], data_[i])) {
				std::swap(data_[parent(i)], data_[i]);
			}
		}

		bool sift_up_changed(size_t i) {
			bool result = false;
			while (i > rooti() && !compare(data_[parent(i)], data_[i])) {
				std::swap(data_[parent(i)], data_[i]);
				result = true;
			}
			return result;
		}

		void sift_down(size_t i) {
			while (i < data_.size()) {
				auto largest = i;

				if (lhs(i) < data_.size() && !compare(data_[largest], data_[lhs(i)])) {
					largest = lhs(i);
				}

				if (rhs(i) < data_.size() && !compare(data_[largest], data_[rhs(i)])) {
					largest = rhs(i);
				}

				if (largest != i) {
					std::swap(data_[i], data_[largest]);
					i = largest;
				} else {
					return;
				}
			}
		}

		bool compare(const T& a, const T& b)
		{
			return static_cast<Compare*>(this)->operator()(a, b);
		}

		size_t parent(size_t idx) const {
			return (idx - 1) / 2;
		}

		size_t lhs(size_t idx) const {
			return idx * 2 + 1;
		}

		size_t rhs(size_t idx) const {
			return idx * 2 + 2;
		}

		size_t rooti() const {
			return 0;
		}

		void sift_fix(size_t i) {
			if (!sift_up_changed(i)) {
				sift_down(i);
			}
		}

	private:
		container_type data_;
	};


}

#endif
