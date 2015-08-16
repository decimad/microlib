//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_CIRCULARBUFFER_HPP_
#define MICROLIB_CIRCULARBUFFER_HPP_
#include <array>

namespace ulib {

	template< typename T, size_t Size >
	struct circular_buffer {
		circular_buffer()
			: front_(0)
		{}

		void reset(T value = 0)
		{
			for(auto& elem : arr_) {
				elem = value;
			}

			front_ = 0;
		}

		void set(T value = 0)
		{
			for(auto& elem : arr_) {
				elem = value;
			}
			front_ = Size;
		}

		void add(T value) {
			arr_[front_%Size] = value;
			++front_;
		}

		T& operator[](size_t index) {
			return arr_[(front_-index)%Size];
		}

		T operator[](size_t index) const {
			return arr_[(front_-index)%Size];
		}

		void offset_all(T off) {
			for(auto& elem : arr_) {
				elem += off;
			}
		}

		const std::array<T, Size>& store() const
		{
			return arr_;
		}

	private:
		std::array<T, Size> arr_;
		size_t front_;
	};

	template< typename T, size_t Size >
	struct circular_averaging_buffer {
		circular_averaging_buffer()
			: front_(0), sum_(0)
		{}

		void reset(T value = 0)
		{
			for(auto& elem : arr_) {
				elem = value;
			}

			front_ = 0;
		}

		void set(T value = 0)
		{
			for(auto& elem : arr_) {
				elem = value;
			}
			front_ = Size;
			sum_ = value * T(Size);
		}

		void add(T value) {
			sum_ -= arr_[front_%Size];
			arr_[front_%Size] = value;
			sum_ += value;
			++front_;
		}

		T average() const {
			return sum_ / T(Size);
		}

		T& operator[](size_t index) {
			return arr_[(front_-index)%Size];
		}

		T operator[](size_t index) const {
			return arr_[(front_-index)%Size];
		}

		void offset_all(T off) {
			for(auto& elem : arr_) {
				elem += off;
			}
			sum_ += Size*off;
		}

	private:
		std::array<T, Size> arr_;
		T sum_;
		size_t front_;
	};

	template< typename T, size_t Size >
	class circular_buffer2 {
	public:
		circular_buffer2()
			: front_(0), back_(0)
		{}

		size_t size() const {
			return back_ - front_;
		}

		bool push(T elem) {
			if (size() != Size) {
				new (ptr(back_)) T(std::move(elem));
				++back_;
				return true;
			} else {
				return false;
			}
		}

		template< typename... Args >
		bool emplace( Args&&... args )
		{
			if (size() != Size) {
				new (ptr(back_)) T(std::forward<Args>(args)...);
				++back_;
				return true;
			} else {
				return false;
			}
		}

		T& front()
		{
			return *ptr(front_);
		}

		const T& front() const
		{
			return *ptr(front_);
		}

		void pop()
		{
			ptr(front_)->~T();
			++front_;
		}

		void clear() {
			while (size()) {
				pop();
			}
		}

		~circular_buffer2()
		{
			clear();
		}

	private:
		
		T* ptr(size_t index) {
			return reinterpret_cast<T*>(&data_[index%Size]);
		}

		const T* ptr(size_t index) const {
			return reinterpret_cast<const T*>(&data_[index%Size]);
		}

		using storage_type = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
		storage_type data_[Size];
		size_t front_;
		size_t back_;
	};

}

#endif
