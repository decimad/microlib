//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_STATIC_UNION_HPP
#define MICROLIB_STATIC_UNION_HPP

#include <utility>
#include <type_traits>
#include <microlib/util.hpp>
#include <microlib/meta.hpp>

namespace ulib {

	namespace experimental {

		template< typename T >
		struct deduce_method_args;

		template< typename Ret, typename ClassType, typename... Args >
		struct deduce_method_args< Ret(ClassType::*)(Args...) >
		{
			using args_list = ulib::meta::list<Args...>;
			using return_type = Ret;
			using class_type = ClassType;
		};

		template< typename T, T Value >
		struct method
		{
			using type = T;
			constexpr static auto value = Value;

			using args_list = typename deduce_method_args<T>::args_list;
			using return_type = typename deduce_method_args<T>::return_type;
			using class_type = typename deduce_method_args<T>::class_type;

			template< typename... Args >
			static return_type call(class_type& cl, Args&&... args) {
				return (cl.*value)(std::forward<Args>(args)...);
			}
		};

#define METHOD(X) \
	::ulib::experimental::method<decltype( X ), X >

	}

	template< typename T, typename S >
	struct case_ {
		using case_type = T;
		using call_type = S;
	};


	template< typename T, size_t Current = 0, typename... Types>
	struct type_to_index;

	template< typename T, size_t Current, typename Type0, typename... Types >
	struct type_to_index< T, Current, Type0, Types... > {
		static const size_t value = std::is_same<Type0, T>::value ? Current : type_to_index<T, Current + 1, Types...>::value;
	};

	template< typename T, size_t current >
	struct type_to_index<T, current>
	{
		static const size_t value = -1;
	};

	template< size_t Index, typename... Types >
	struct index_to_type;

	template< size_t Index, typename Type0, typename... Types >
	struct index_to_type< Index, Type0, Types... >
	{
		using type = typename index_to_type< Index - 1, Types... >::type;
	};

	template< typename Type0, typename... Types >
	struct index_to_type< 0, Type0, Types... >
	{
		using type = Type0;
	};

	template< size_t Index, typename... Types >
	using index_to_type_t = typename index_to_type<Index, Types...>::type;

	template< typename... Types >
	struct static_union {
	private:
		static constexpr size_t Size = sizeof...(Types);

		template< size_t Index >
		void destruct_impl(size_t index, std::integral_constant<size_t, Index>) {
			if (Index == index) {
				using type = typename index_to_type<Index, Types...>::type;
				auto ptr = reinterpret_cast<type*>(&storage_);
				ptr->~type();
				current_type = -1;
			} else {
				destruct_impl(index, std::integral_constant<size_t, Index + 1>());
			}
		}

		void destruct_impl(size_t index, std::integral_constant<size_t, sizeof...(Types)>) {
			(void) index;
			current_type = -1;
		}

		void destruct(size_t index) {
			destruct_impl(index, std::integral_constant<size_t, 0>());
		}

	public:
		void clear() {
			if (current_type != size_t(-1)) destruct(current_type);
		}

		template< typename ToType, typename... Args >
		ToType& to_type(Args&&... args)
		{
			static_assert(type_to_index<ToType, 0, Types...>::value != -1, "Not my type.");

			if (current_type != size_t(-1)) {
				destruct(current_type);
			}

			// Need to set before, because the created type might instantly switch
			current_type = type_to_index<ToType, 0, Types...>::value;
			new (&storage_) ToType(std::forward<Args>(args)...);


			return *reinterpret_cast<ToType*>(&storage_);
		}

		template< typename T >
		bool is() const
		{
			static_assert(type_to_index<T, 0, Types...>::value != -1, "Not my type!");
			return current_type == type_to_index<T, 0, Types...>::value;
		}

		template< typename T >
		T& as() {
			static_assert(type_to_index<T, 0, Types...>::value != -1, "Not my type!");
			return *reinterpret_cast<T*>(&storage_);
		}

		template< typename T >
		const T& as() const {
			static_assert(type_to_index<T, 0, Types...>::value != -1, "Not my type!");
			return *reinterpret_cast<const T*>(&storage_);
		}

		template< typename... Cases, typename... Args >
		void dispatch(Args&&... args)
		{
			dispatch_impl(meta::list<Cases...>(), std::forward<Args>(args)...);
		}

		template< typename... Cases, typename... Args >
		void dispatch_self(Args&&... args)
		{
			dispatch_self_impl(meta::list<Cases...>(), std::forward<Args>(args)...);
		}


	private:
		template< typename Case0, typename... Cases, typename... Args >
		void dispatch_impl(meta::list<Case0, Cases...>, Args&&... args )
		{
			using case_type = typename Case0::case_type;
			if (type_to_index<case_type, 0, Types...>::value == current_type) {
				using call_type = typename Case0::call_type;
				call_type::call(std::forward<Args>(args)..., as<case_type>());
			} else {
				return dispatch_impl(meta::list<Cases...>(), std::forward<Args>(args)...);
			}
		}

		template< typename Case0, typename... Cases, typename... Args >
		void dispatch_self_impl(meta::list<Case0, Cases...>, Args&&... args)
		{
			using case_type = typename Case0::case_type;
			if (type_to_index<case_type, 0, Types...>::value == current_type) {
				using call_type = typename Case0::call_type;
				call_type::call(as<case_type>(), std::forward<Args>(args)...);
			} else {
				return dispatch_self_impl(meta::list<Cases...>(), std::forward<Args>(args)...);
			}
		}

		template< typename... Args >
		void dispatch_impl(meta::list<>, Args&&... args)
		{}

		template< typename... Args >
		void dispatch_self_impl(meta::list<>, Args&&... args)
		{
		}

		template< typename Iface, size_t Index >
		Iface* get_interface_impl(size_t current_type, std::integral_constant<size_t, Index>) {
			if (Index == current_type) {
				return &as<index_to_type_t<Index, Types...>>();
			} else {
				return get_interface_impl<Iface>(current_type, std::integral_constant<size_t, Index+1>());
			}
		}

		template<typename Iface>
		Iface* get_interface_impl(size_t current_type, std::integral_constant<size_t, Size>) {
			(void) current_type;
			return nullptr;
		}

	public:
		template< typename Iface >
		Iface* get_interface()
		{
			return get_interface_impl<Iface>(current_type, std::integral_constant<size_t, 0>());
		}

		static_union()
			: current_type(-1)
		{}

		~static_union()
		{
			clear();
		}

	private:
		using storage_type = typename std::aligned_storage<max_size<Types...>::value, max_alignment<Types...>::value>::type;
		storage_type storage_;

		size_t current_type = -1;
	};

}

#endif
