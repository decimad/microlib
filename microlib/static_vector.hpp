#ifndef MICROLIB_STATIC_VECTOR_HPP__
#define MICROLIB_STATIC_VECTOR_HPP__

#include <type_traits>
#include <algorithm>
#include <microlib/util.hpp>

namespace ulib {

	namespace detail {

		template< size_t Size >
		struct auto_size_type {
			using type = typename std::conditional< (Size <= std::numeric_limits<unsigned char>::max()), unsigned char,
				typename std::conditional< (Size <= std::numeric_limits<unsigned short>::max()), unsigned short, unsigned int >::type >::type;
		};

		template< size_t Size >
		using auto_size_type_t = typename auto_size_type<Size>::type;


		// We need one-based arrays for efficient heaps.
		// For this we could just ignore the first element of the array (still needing a specialization, since that element must not be constructed for non-pod types),
		// however, if the current size fits into this spot, we can avoid wasting memory and still profit from one based arrays in heaps.
		// That's why we have specializations on the involved sizes. The template works on sizes and aligns to reduce type bloat.


		template< size_t SizeOfT, size_t AlignOfT, size_t Size, size_t Base, bool InArraySize = (SizeOfT*Base >= sizeof(auto_size_type_t<Size>) && AlignOfT >= alignof(auto_size_type_t<Size>)) >
		struct static_vector_storage {
			using element_storage_type = typename std::aligned_storage<SizeOfT, AlignOfT>::type;
			using size_type = auto_size_type_t<Size>;

			size_type& size_ref() {
				return size_;
			}

			const size_type& size_ref() const {
				return size_;
			}

			void* data()
			{
				return static_cast<void*>(&data_[0]);
			}

			const void* data() const
			{
				return static_cast<const void*>(&data_[0]);
			}

			size_type capacity() const
			{
				return Size - Base;
			}

			size_type size_;
			element_storage_type data_[Size];
		};

		template< size_t SizeOfT, size_t AlignOfT, size_t Size, size_t Base>
		struct static_vector_storage< SizeOfT, AlignOfT, Size, Base, true > {
			using element_storage_type = typename std::aligned_storage<SizeOfT, AlignOfT>::type;
			using size_type = auto_size_type_t<Size>;

			size_type& size_ref() {
				return *reinterpret_cast<size_type*>(data());
			}

			const size_type& size_ref() const {
				return *reinterpret_cast<const size_type*>(data());
			}

			void* data()
			{
				return &data_[0];
			}

			const void* data() const
			{
				return &data_[0];
			}

			size_type capacity() const
			{
				return Size - Base;
			}

			element_storage_type data_[Size];
		};

	}


	template< typename T, size_t Size, size_t Base = 0 >
	class static_vector : private detail::static_vector_storage<sizeof(T), alignof(T), Size, Base> {
	public:
		using iterator = T*;
		using const_iterator = const T*;
		using storage = detail::static_vector_storage<sizeof(T), alignof(T), Size, Base>;
		using size_type = typename storage::size_type;

		static constexpr size_type Elements = Size - Base;

		static_vector()
		{
			storage::size_ref() = 0;
		}

		bool push_back(T&& val)
		{
			if (size() != capacity()) {
				new (data() + Base + size()) T(std::move(val));
				++storage::size_ref();
				return true;
			} else {
				return false;
			}
		}

		bool push_back(const T& val)
		{
			if (size() != capacity()) {
				new (data() + Base + size()) T(val);
				++storage::size_ref();
				return true;
			} else {
				return false;
			}
		}

		template< typename... Args >
		bool emplace_back(Args&&... args)
		{
			if (size() != capacity()) {
				new (data() + Base + size()) T(std::forward<Args>(args)...);
				++storage::size_ref();
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
				--storage::size_ref();
				*(data() + (storage::size_ref()))->~T();
			}
		}

		void pop_back()
		{
			(data() + size() + Base)->~T();
			--storage::size_ref();
		}

		iterator begin()
		{
			return data() + Base;
		}

		iterator end()
		{
			return data() + Base + size();
		}

		const_iterator begin() const
		{
			return data() + Base;
		}

		const_iterator end() const
		{
			return data() + Base + size();
		}

		T* data() {
			return static_cast<T*>(storage::data());
		}

		const T* data() const {
			return static_cast<const T*>(storage::data());
		}

		T& operator[](size_t index)
		{
			return *(data() + index);
		}

		const T& operator[](size_t index) const
		{
			return *(data() + index);
		}

		size_type size() const
		{
			return storage::size_ref();
		}

		size_type capacity() const
		{
			return storage::capacity();
		}

		void clear()
		{
			const auto currsize = size();
			for (auto& elem : *this) {
				elem.~T();
				//(data()+i)->~T();
			}
			storage::size_ref() = 0;
		}
	};

}

#endif
