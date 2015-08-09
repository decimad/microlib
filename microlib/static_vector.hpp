#ifndef STM_UTIL_STATIC_VECTOR_HPP__
#define STM_UTIL_STATIC_VECTOR_HPP__

#include <type_traits>
#include <algorithm>

namespace util {

	template< typename T, size_t Size >
	class static_vector {
	public:
		using iterator = T*;
		using const_iterator = const T*;

		static_vector()
			: size_(0)
		{}

		bool push_back(T elem)
		{
			if (size_ == Size) {
				return false;
			} else {
				new (&data_[size_]) T(std::move(elem));
				++size;
			}
		}

		template< typename... Args >
		bool emplace_back(Args&&... args)
		{
			if (size_ != Size) {
				new (&data_[size_]) T(std::forward<Args>(args)...);
				++size_;
				return true;
			} else {
				return false;
			}
		}

		void erase(const_iterator where)
		{
			std::rotate(where, where + 1, end);
			pop_back();
		}

		void pop_front()
		{
			if (size()) {
				std::rotate(begin(), begin() + 1, end());
				*(data() + (size_ - 1))->~T();
				--size_;
			}
		}

		void pop_back()
		{
			--size_;
			reinterpret_cast<T*>(&data_[size_])->~T();
		}

		iterator begin()
		{
			return data();
		}

		iterator end()
		{
			return data() + size_;
		}

		const_iterator begin() const
		{
			return data();
		}

		const_iterator end() const
		{
			return data() + size_;
		}

		T* data()
		{
			return reinterpret_cast<T*>(&data_[0]);
		}

		const T* data() const
		{
			return reinterpret_cast<T*>(&data_[0]);
		}

		T& operator[](size_t index)
		{
			return *(data() + index);
		}

		const T& operator[](size_t index) const
		{
			return *(data() + index);
		}

		size_t size() const
		{
			return size_;
		}

		size_t capacity() const
		{
			return Size;
		}

		void clear()
		{
			for (size_t i = 0; i < size_; ++i) {
				(data() + i)->~T();
			}
			size_ = 0;
		}

	private:
		using element_storage_type = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
		element_storage_type data_[Size];
		size_t size_;
	};


}

#endif
