//          Copyright Michael Steinberg 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef ULIB_META_TLIST_HPP___
#define ULIB_META_TLIST_HPP___

#include "meta_common.hpp"
#include <cstddef>
#include <type_traits>

namespace ulib::meta
{
    using std::size_t;

    namespace type_list
    {

        struct error_type
        {
        };

        template<typename T>
        constexpr bool is_type_list = concepts::TypeList<T>;

        template <concepts::TypeList List>
        constexpr size_t size = common::size_v<List>;

        template <concepts::TypeList List>
        constexpr bool is_empty = common::is_empty<List>::value;

        // clear< List >
        // remove all elements
        template <concepts::TypeList List>
        using clear = typename common::clear<List>::type;

        // reverse< List >
        // reverse the elements of a type_list
        template <concepts::TypeList List>
        using reverse = common::reverse_t<List>;

        // push_front< List, Arg >
        // add new element Arg to the front of the type_list List
        template <concepts::TypeList List, typename Arg>
        using push_front = common::push_front_tlist_t<List, Arg>;

        // pop_front< List >
        // remove first element of type_list List (if the list is empty, the result is error_type)
        template <concepts::TypeList List>
        using pop_front = common::pop_front_t<List>;

        // push_back< List, Arg >
        // add new element Arg to the ent of the type_list List
        template <concepts::TypeList List, typename Arg>
        using push_back = common::push_back_tlist_t<List, Arg>;

        // pop_back< List >
        // remove last element of the type_list List (if the list is empty, the result is error_type)
        template <concepts::TypeList List>
        using pop_back = typename common::pop_back<List>::type;

        // replace_front_t<List, Elem>
        template <concepts::TypeList List, typename Elem>
        using replace_front = push_front<pop_front<List>, Elem>;

        // replace_back<List, Elem>
        template <concepts::TypeList List, typename Elem>
        using replace_back = push_back<pop_back<List>, Elem>;

        //
        template <concepts::TypeList List>
        requires(size<List> > 0)
        using front = common::front_tlist_t<List>;

        // back< List >
        template <concepts::TypeList List>
        requires(size<List> > 0)
        using back = common::back_tlist_t<List>;

        // split< List, N >
        // split list in front of Nth element
        template <concepts::TypeList List, size_t N>
        requires(size<List> >= N)
        using split = common::split_t<List, N>;

        // concat< List, List >
        template<concepts::TypeList List1, concepts::TypeList List2>
        using concat = common::concat_t<List1, List2>;

        // insert< List, N, List >
        template<concepts::TypeList Dest, size_t N, concepts::TypeList Source>
        requires (size<Dest> >= N)
        using insert = typename common::insert<Dest, N, Source>::type;

        // transform< List, Transformer >
        template <typename List, template <typename> class Transformer>
        using transform = common::transform_tlist_t<List, Transformer>;

        // get< List, Where >
        template<concepts::TypeList List, size_t Where>
        requires (size<List> > Where)
        using get = common::get_tlist_t<List, Where>;

        // set< List, Where, T >
        template<concepts::TypeList List, size_t Where, typename T>
        requires (size<List> > Where)
        using set = common::set_tlist_t<List, Where, T>;

    }

    namespace detail {

        template<typename Tlist>
        struct tlist_front_back_base
        {
            using front = type_list::front<Tlist>;
            using back  = type_list::back<Tlist>;
        };

        template<template<typename...> typename Tlist>
        struct tlist_front_back_base< Tlist<> >
        {

        };

    }

    template <typename... Elems>
    struct tlist : public detail::tlist_front_back_base<tlist<Elems...>>
    {
        static constexpr size_t size = type_list::size<tlist>;

        static constexpr bool is_empty = (size == 0);

        using cleared = type_list::clear<tlist>;

        template<typename T>
        using push_front = type_list::push_front<tlist, T>;

        template<typename T>
        using push_back  = type_list::push_back<tlist, T>;

        using reverse = type_list::reverse<tlist>;

        template<concepts::TypeList Other>
        using concat = type_list::concat<tlist, Other>;

        template<template <typename T> typename Transformer>
        using transform = type_list::transform<tlist, Transformer>;

        template<template< typename... > typename OtherListType>
        using as = OtherListType<Elems...>;

        template<size_t Where>
        using get = type_list::get<tlist, Where>;

        template<size_t Where, typename T>
        using set = type_list::set<tlist, Where, T>;
    };

    template <typename Type, Type... Elements>
    using integral_tlist = tlist<std::integral_constant<Type, Elements>...>;

} // namespace ulib::mete

#endif /* ULIB_META_TLIST_HPP___ */
