//          Copyright Michael Steinberg 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef ULIB_META_VLIST_HPP___
#define ULIB_META_VLIST_HPP___

#include "meta_common.hpp"
#include <cstddef>
#include <type_traits>

namespace ulib::meta
{
    namespace value_list {

        template<typename T>
        constexpr bool is_value_list = concepts::ValueList<T>;

        template<concepts::ValueList List>
        constexpr size_t size = common::size_v<List>;

        template<concepts::ValueList List>
        constexpr bool is_empty = common::is_empty_v<List>;

        template<concepts::ValueList List>
        using clear = common::clear_t<List>;

        template<concepts::ValueList List>
        requires(size<List> > 0)
        static constexpr common::value_type_t<List> front = common::front_vlist_v<List>;

        template<concepts::ValueList List>
        requires(size<List> > 0)
        static constexpr common::value_type_t<List> back = common::back_vlist_v<List>;

        template<concepts::ValueList List, common::value_type_t<List> Value>
        using push_front = common::push_front_vlist_t<List, Value>;

        template<concepts::ValueList List>
        requires(size<List> > 0)
        using pop_front = common::pop_front_t<List>;

        template<concepts::ValueList List, common::value_type_t<List> Value>
        using push_back = common::push_back_vlist_t<List, Value>;

        template<concepts::ValueList List>
        requires(size<List> > 0)
        using pop_back = common::pop_back_t<List>;

        template<concepts::ValueList List1, concepts::ValueList List2>
        using concat = common::concat_t<List1, List2>;

        template<concepts::ValueList List>
        using reverse = common::reverse_t<List>;

        template<concepts::ValueList List, template< auto Value > class Transformer>
        using transform = common::transform_vlist_t<List, Transformer>;

        template<concepts::ValueList List, size_t N>
        requires (size<List> > N)
        static constexpr auto get = common::get_vlist_v<List, N>;

        template<concepts::ValueList List, size_t N, common::value_type_t<List> Arg>
        requires (size<List> > N)
        using set = common::set_vlist_t<List, N, Arg>;

        template<concepts::ValueList List, typename Body, size_t Index = 0, size_t Size = size<List>>
        struct for_each_t
        {
            using type = typename for_each_t<List, typename Body::template run<Index, get<List, Index>>, Index+1, Size>::type;
        };

        template<concepts::ValueList List, typename Body, size_t Size>
        struct for_each_t<List, Body, Size, Size>
        {
            using type = Body;
        };

        template<concepts::ValueList List, typename Body>
        using for_each = typename for_each_t<List, Body>::type;
    }

    namespace detail {

        template<concepts::ValueList Vlist>
        struct vlist_front_back_base
        {
            static constexpr auto front = value_list::front<Vlist>;
            static constexpr auto back  = value_list::back<Vlist>;

            using pop_front = value_list::pop_front<Vlist>;
            using pop_back  = value_list::pop_back<Vlist>;
        };

        template<template<typename T, T... Vs> typename Vlist, typename T>
        struct vlist_front_back_base< Vlist<T> >
        {
        };

    }

    template<typename T, T... Vs>
    struct vlist : public detail::vlist_front_back_base<vlist<T, Vs...>>
    {
        static constexpr size_t size = value_list::size<vlist>;

        static constexpr bool is_empty = (size == 0);

        using cleared = value_list::clear<vlist>;

        template<T Value>
        using push_front = value_list::push_front<vlist, Value>;

        template<T Value>
        using push_back  = value_list::push_back<vlist, Value>;

        using reverse = value_list::reverse<vlist>;

        template<concepts::ValueList Other>
        using concat = value_list::concat<vlist, Other>;

        template<template <auto X> typename Transformer>
        using transform = value_list::transform<vlist, Transformer>;

        template<template<typename X, X...> typename OtherListType>
        using convert = OtherListType<T, Vs...>;

        template<size_t N>
        static constexpr T get = value_list::get<vlist, N>;

        template<size_t N, common::value_type_t<vlist> Arg>
        using set = value_list::set<vlist, N, Arg>;
    };

} // namespace ulib::meta

#endif /* ULIB_META_VLIST_HPP___ */
