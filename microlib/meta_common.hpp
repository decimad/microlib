//          Copyright Michael Steinberg 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef ULIB_META_COMMON_HPP_
#define ULIB_META_COMMON_HPP_

#include <type_traits>
#include <cstddef>

namespace ulib::meta
{
    namespace concepts {

        // TypeList concept

        namespace detail {
            template<typename T>
            struct is_type_list
            {
                static constexpr bool value = false;
            };

            template<template<typename...> typename XList, typename... Ts>
            struct is_type_list<XList<Ts...>>
            {
                static constexpr bool value = true;
            };
        }

        // ValueList concept

        template<typename T>
        concept TypeList = detail::is_type_list<T>::value;

        namespace detail {
            template<typename T>
            struct is_value_list
            {
                static constexpr bool value = false;
            };

            template<template<typename T, T...> typename XList, typename T, T... Vs>
            struct is_value_list<XList<T, Vs...>>
            {
                static constexpr bool value = true;
            };
        }

        template<typename T>
        concept ValueList = detail::is_value_list<T>::value;

        // MetaObject concept

        template<typename T>
        concept MetaObject = ValueList<T> || TypeList<T>;
    }

    namespace common {

        template<concepts::TypeList List1, concepts::TypeList List2>
        struct same_type_list_type
        {
            static constexpr bool value = false;
        };

        template<template<typename...> typename XList, typename... Ts1, typename... Ts2>
        struct same_type_list_type<XList<Ts1...>, XList<Ts2...>>
        {
            static constexpr bool value = true;
        };

        template<concepts::TypeList List1, concepts::TypeList List2>
        static constexpr bool same_type_list_type_v = same_type_list_type<List1, List2>::value;

        template<concepts::ValueList List1, concepts::ValueList List2>
        struct same_value_list_type
        {
            static constexpr bool value = false;
        };

        template<template<typename X, X...> typename XList, typename T, T... Vs1, T... Vs2>
        struct same_value_list_type<XList<T, Vs1...>, XList<T, Vs2...>>
        {
            static constexpr bool value = true;
        };

        template<concepts::ValueList List1, concepts::ValueList List2>
        static constexpr bool same_value_list_type_v = same_value_list_type<List1, List2>::value;

        //
        // size
        //
        template <concepts::MetaObject Object>
        struct size;

        template <template <typename...> typename XList, typename... Ts>
        struct size<XList<Ts...>>
        {
            static constexpr size_t value = sizeof...(Ts);
        };

        template<template <typename X, X...> typename XList, typename Type, Type... Vs>
        struct size<XList<Type, Vs...>>
        {
            static constexpr size_t value = sizeof...(Vs);
        };

        template<concepts::MetaObject Object>
        constexpr size_t size_v = size<Object>::value;

        //
        // is_empty
        //

        template <concepts::MetaObject T>
        struct is_empty
        {
            static constexpr bool value = (size<T>::value == 0);
        };

        template <concepts::MetaObject T>
        static constexpr bool is_empty_v = is_empty<T>::value;

        //
        // clear
        //

        template <concepts::MetaObject>
        struct clear;

        template <template <typename...> typename XList, typename... Ts>
        struct clear<XList<Ts...>>
        {
            using type = XList<>;
        };

        template<template <typename X, X...> typename XList, typename T, T... Vs>
        struct clear<XList<T, Vs...>>
        {
            using type = XList<T>;
        };

        template <concepts::MetaObject Object>
        using clear_t = typename clear<Object>::type;

        //
        // push_front
        //

        template <concepts::TypeList List, typename Arg>
        struct push_front_tlist;

        template <template <typename...> typename XList, typename Arg, typename... Ts>
        struct push_front_tlist<XList<Ts...>, Arg>
        {
            using type = XList<Arg, Ts...>;
        };

        template <concepts::TypeList List, typename Arg>
        using push_front_tlist_t = typename push_front_tlist<List, Arg>::type;

        template<concepts::ValueList List>
        struct value_type;

        template<template <typename X, X...> typename XList, typename T, T... Vs>
        struct value_type<XList<T, Vs...>>
        {
            using type = T;
        };

        template<concepts::ValueList List>
        using value_type_t = typename value_type<List>::type;

        template <concepts::ValueList List, value_type_t<List> Element>
        struct push_front_vlist;

        template <template <typename X, X...> typename XList, typename T, T... Vs, T Arg>
        struct push_front_vlist<XList<T, Vs...>, Arg>
        {
            using type = XList<T, Arg, Vs...>;
        };

        template <concepts::ValueList List, value_type_t<List> V>
        using push_front_vlist_t = typename push_front_vlist<List, V>::type;

        //
        // push_back
        //

        template <concepts::TypeList List, typename Arg>
        struct push_back_tlist;

        template <template <typename...> typename XList, typename Arg, typename... Ts>
        struct push_back_tlist<XList<Ts...>, Arg>
        {
            using type = XList<Ts..., Arg>;
        };

        template <concepts::TypeList List, typename Arg>
        using push_back_tlist_t = typename push_back_tlist<List, Arg>::type;

        template <concepts::ValueList List, value_type_t<List> Element>
        struct push_back_vlist;

        template <template <typename X, X...> typename XList, typename T, T... Vs, T Arg>
        struct push_back_vlist<XList<T, Vs...>, Arg>
        {
            using type = XList<T, Vs..., Arg>;
        };

        template <concepts::ValueList List, value_type_t<List> Arg>
        using push_back_vlist_t = typename push_back_vlist<List, Arg>::type;

        //
        // reverse
        //

        template <concepts::MetaObject List>
        struct reverse;

        template<concepts::MetaObject Object>
        using reverse_t = typename reverse<Object>::type;

        template <template <typename...> typename XList>
        struct reverse<XList<>>
        {
            using type = XList<>;
        };

        template <template <typename...> typename XList, typename T0, typename... Ts>
        struct reverse<XList<T0, Ts...>>
        {
            using type = push_back_tlist_t<reverse_t<XList<Ts...>>, T0>;
        };

        template <template <typename X, X...> typename XList, typename T>
        struct reverse<XList<T>>
        {
            using type = XList<T>;
        };

        template <template <typename T, T...> typename XList, typename T, T V0, T... Vs>
        struct reverse<XList<T, V0, Vs...>>
        {
            using type = push_back_vlist_t<reverse_t<XList<T, Vs...>>, V0>;
        };

        //
        // pop_front
        //

        template <concepts::MetaObject Object>
        requires(size_v<Object> > 0)
        struct pop_front;

        template <template <typename...> typename XList, typename T0, typename... Ts>
        struct pop_front<XList<T0, Ts...>>
        {
            using type = XList<Ts...>;
        };

        template <template <typename T, T...> typename XList, typename T, T V0, T... Vs>
        struct pop_front<XList<T, V0, Vs...>>
        {
            using type = XList<T, Vs...>;
        };

        template <concepts::MetaObject Object>
        using pop_front_t = typename pop_front<Object>::type;

        //
        // pop_back
        // Todo: Implementation using "reverse" is probably inefficient...
        //

        template <concepts::MetaObject Object>
        requires(size_v<Object> > 0)
        struct pop_back
        {
            using type = reverse_t<pop_front_t<reverse_t<Object>>>;
        };

        template <concepts::MetaObject Object>
        using pop_back_t = typename pop_back<Object>::type;

        //
        // front_tlist
        //

        template <concepts::MetaObject List>
        requires(size_v<List> > 0)
        struct front_tlist;

        template <template <typename...> typename XList, typename T0, typename... Ts>
        struct front_tlist<XList<T0, Ts...>>
        {
            using type = T0;
        };

        template <concepts::MetaObject List>
        using front_tlist_t = typename front_tlist<List>::type;

        //
        // front_vlist
        //
        template <concepts::ValueList List>
        requires(size_v<List> > 0)
        struct front_vlist;

        template <template <typename T, T...> typename XList, typename T, T V0, T... Vs>
        struct front_vlist<XList<T, V0, Vs...>>
        {
            static constexpr T value = V0;
        };

        template <concepts::ValueList List>
        static constexpr value_type_t<List> front_vlist_v = front_vlist<List>::value;

        namespace detail {

            template<typename... Ts>
            struct last_type
            {
                // Use a fold-expression to fold the comma operator over the parameter pack.
                using type = typename decltype((std::type_identity<Ts>{}, ...))::type;
            };

            template<typename T, T... Vs>
            struct last_value
            {
                static constexpr T value = (Vs, ...);
            };

        }

        //
        // back_tlist
        //
        template <concepts::TypeList List>
        requires(size_v<List> > 0)
        struct back_tlist;

        template <template <typename...> typename XList, typename... Ts>
        struct back_tlist<XList<Ts...>>
        {
            using type = typename detail::last_type<Ts...>::type;
        };

        template <concepts::TypeList List>
        using back_tlist_t = typename back_tlist<List>::type;

        //
        // back_vlist
        //

        template <concepts::ValueList List>
        requires(size_v<List> > 0)
        struct back_vlist;

        template <template <typename X, X...> typename List, typename T, T... Vs>
        struct back_vlist<List<T, Vs...>>
        {
            static constexpr T value = detail::last_value<T, Vs...>::value;
        };

        template<concepts::ValueList List>
        static constexpr common::value_type_t<List> back_vlist_v = back_vlist<List>::value;

        //
        // concat
        //

        template<concepts::MetaObject List1, concepts::MetaObject List2>
        requires (same_type_list_type_v<List1, List2> || same_value_list_type_v<List1, List2>)
        struct concat;

        template< template<typename...> typename XList, typename... Ts1, typename... Ts2 >
        struct concat<XList<Ts1...>, XList<Ts2...>>
        {
            using type = XList<Ts1..., Ts2...>;
        };

        template<template<typename X, X...> typename XList, typename T, T... Vs1, T... Vs2 >
        struct concat<XList<T, Vs1...>, XList<T, Vs2...>>
        {
            using type = XList<T, Vs1..., Vs2...>;
        };

        template<concepts::MetaObject List1, concepts::MetaObject List2>
        using concat_t = typename concat<List1, List2>::type;

        //
        // transform_tlist
        //

        template<concepts::TypeList List, template <typename T> typename Transformer>
        struct transform_tlist;

        template<template<typename...> typename XList, template <typename T> typename XTransformer, typename... Ts>
        struct transform_tlist<XList<Ts...>, XTransformer>
        {
            using type = XList<typename XTransformer<Ts>::type...>;
        };

        template<concepts::TypeList List, template <typename T> typename Transformer>
        using transform_tlist_t = typename transform_tlist<List, Transformer>::type;

        //
        // transform_vlist
        //

        template<concepts::ValueList List, template <auto Value> typename Transformer>
        struct transform_vlist;

        template<template<typename T, T...> typename List, template <auto X> typename XTransformer, typename T, T... Vs>
        struct transform_vlist<List<T, Vs...>, XTransformer>
        {
            using type = List<T, XTransformer<Vs>::value...>;
        };

        template<concepts::ValueList List, template <auto Value> typename Transformer>
        using transform_vlist_t = typename transform_vlist<List, Transformer>::type;

        //
        // get
        //

        namespace detail {
            // detail get nth type
            template<size_t N, typename... Types>
            requires(N < sizeof...(Types))
            struct get_tlist;

            template<typename T0, typename... Ts>
            struct get_tlist<0, T0, Ts...>
            {
                using type = T0;
            };

            template<size_t N, typename T0, typename... Ts>
            struct get_tlist<N, T0, Ts...>
            {
                using type = typename get_tlist<N-1, Ts...>::type;
            };

            // detail get nth value
            template<size_t N, typename T, T... Vs>
            requires(N < sizeof...(Vs))
            struct get_vlist;

            template<typename T, T V0, T... Vs>
            struct get_vlist<0, T, V0, Vs...>
            {
                static constexpr T value = V0;
            };

            template<size_t N, typename T, T V0, T... Vs>
            struct get_vlist<N, T, V0, Vs...>
            {
                static constexpr T value = get_vlist<N-1, T, Vs...>::value;
            };
        }

        //
        // get_tlist
        //
        template<concepts::TypeList List, size_t N>
        requires (size<List>::value > N)
        struct get_tlist;

        template<template <typename...> typename XList, typename... Ts, size_t N>
        struct get_tlist<XList<Ts...>, N>
        {
            using type = typename detail::get_tlist<N, Ts...>::type;
        };

        template<concepts::TypeList List, size_t N>
        using get_tlist_t = typename get_tlist<List, N>::type;

        //
        // get_vlist
        //

        template<concepts::ValueList List, size_t N>
        requires (size<List>::value > N)
        struct get_vlist;

        template<template <typename X, X...> typename XList, typename T, T... Vs, size_t N>
        struct get_vlist<XList<T, Vs...>, N>
        {
            static constexpr T value = detail::get_vlist<N, T, Vs...>::value;
        };

        template<concepts::ValueList List, size_t N>
        static constexpr auto get_vlist_v = get_vlist<List, N>::value;

        //
        // split
        //

        namespace detail {

            template<concepts::MetaObject Lhs, concepts::MetaObject Rhs, size_t Where>
            struct split_t;

            template<concepts::TypeList Lhs, concepts::TypeList Rhs, size_t Where>
            struct split_t<Lhs, Rhs, Where> : public split_t<push_back_tlist_t<Lhs, front_tlist_t<Rhs>>, pop_front_t<Rhs>, Where-1>
            {};


            template<concepts::ValueList Lhs, concepts::ValueList Rhs, size_t Where>
            struct split_t<Lhs, Rhs, Where> : public split_t<push_back_vlist_t<Lhs, front_vlist_v<Rhs>>, pop_front_t<Rhs>, Where-1>
            {};

            template<concepts::MetaObject Lhs, concepts::MetaObject Rhs>
            struct split_t<Lhs, Rhs, 0>
            {
                using lhs = Lhs;
                using rhs = Rhs;
            };

        }

        template<concepts::MetaObject List, size_t Where>
        requires (size<List>::value >= Where)
        struct split_t : public detail::split_t<clear_t<List>, List, Where>
        {
        };

        //
        // set
        //
        template<concepts::TypeList List, size_t Where, typename T, typename X = split_t<List, Where>>
        struct set_tlist
        {
            using type = concat_t<typename X::lhs, push_front_tlist_t<pop_front_t<typename X::rhs>, T>>;
        };

        template<concepts::TypeList List, size_t Where, typename T>
        using set_tlist_t = typename set_tlist<List, Where, T>::type;

        template<concepts::ValueList List, size_t Where, value_type_t<List> V, typename X = split_t<List, Where>>
        struct set_vlist
        {
            using type = concat_t<typename X::lhs, push_front_vlist_t<pop_front_t<typename X::rhs>, V>>;
        };

        template<concepts::ValueList List, size_t Where, value_type_t<List> V, typename X = split_t<List, Where>>
        using set_vlist_t = typename set_vlist<List, Where, V>::type;

        //
        // insert
        //
        template<concepts::TypeList Dst, size_t Where, concepts::TypeList Src, typename Pair = split_t<Dst, Where>>
        requires (size<Dst>::value >= Where)
        struct insert {
            using type = concat_t< concat_t< typename Pair::lhs, Src >, typename Pair::rhs >;
        };
    }
}

#endif /* ULIB_META_COMMON_HPP_ */
