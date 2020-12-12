//          Copyright Michael Steinberg 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../meta_tlist.hpp"
#include "../meta_vlist.hpp"
#include <type_traits>

namespace ulib::meta::verification {

    template<typename... Ts>
    struct are_same {
        static constexpr bool value = true;
    };

    template<typename T0, typename T1, typename... Ts>
    struct are_same<T0, T1, Ts...>
    {
        static constexpr bool value = std::is_same_v<T0, T1> && are_same<T1, Ts...>::value;
    };

    template<typename... Ts>
    static constexpr bool are_same_v = are_same<Ts...>::value;

    template<auto... Vs>
    struct are_equal {
        static constexpr bool value = true;
    };

    template<auto V0, auto V1, auto... Vs>
    struct are_equal<V0, V1, Vs...> {
        static constexpr bool value = (V0 == V1) && are_equal<V1, Vs...>::value;
    };

    template<auto... Vs>
    static constexpr bool are_equal_v = are_equal<Vs...>::value;

    using meta::tlist;
    using meta::vlist;

    //
    // Concepts
    //

    template< typename... Types >
    struct custom_tlist {};

    template< typename T, T... values >
    struct custom_vlist {};

    static_assert(
        type_list::is_type_list<tlist<>>
    );

    static_assert(
        type_list::is_type_list<custom_tlist<>>
    );

    static_assert(
        value_list::is_value_list<vlist<int>>
    );

    static_assert(
        value_list::is_value_list<custom_vlist<int, 1, 2, 3>>
    );

    //
    // size
    //

    static_assert(
        (type_list::size<tlist<>> == 0) && (tlist<>::size == 0)
    );

    static_assert(
        (type_list::size<tlist<int>> == 1) && (tlist<int>::size == 1)
    );

    static_assert(
        (type_list::size<tlist<int,float,double>> == 3) && (tlist<int,float,double>::size == 3)
    );

    static_assert(
        (value_list::size<vlist<int>> == 0) && (vlist<int>::size == 0)
    );

    static_assert(
        (type_list::size<tlist<int>> == 1) && (tlist<int>::size == 1)
    );

    static_assert(
        (type_list::size<tlist<int,float,double>> == 3) && (tlist<int,float,double>::size == 3)
    );

    //
    // is_empty
    //

    static_assert(
        type_list::is_empty<tlist<>> && tlist<>::is_empty
    );

    static_assert(
        !type_list::is_empty<tlist<int>> && !tlist<int>::is_empty
    );

    static_assert(
        value_list::is_empty<vlist<int>> && vlist<int>::is_empty
    );

    static_assert(
        !value_list::is_empty<vlist<int, 1>> && !vlist<int, 1>::is_empty
    );

    //
    // clear
    //

    static_assert(
        tlist<int>::cleared::is_empty
    );

    static_assert(
        vlist<int, 1>::cleared::is_empty
    );

    //
    // front
    //

    static_assert(
        are_same_v<
            type_list::front<tlist<int,float>>,
            tlist<int,float>::front,
            int
        >
    );

    static_assert(
        value_list::front<vlist<int, 1, 2>> == 1 &&
        vlist<int, 1, 2>::front == 1
    );

    //
    // back
    //

    static_assert(
        are_same_v<
            type_list::back<tlist<int,float>>,
            tlist<int,float>::back,
            float
        >
    );

    static_assert(
        value_list::back<vlist<int, 1, 2>> == 2 &&
        vlist<int, 1, 2>::back == 2
    );

    //
    // push_front
    //

    static_assert(
        are_same_v<
            type_list::push_front<tlist<>, int>,
            tlist<>::push_front<int>,
            tlist<int>
        >
    );

    static_assert(
        are_same_v<
            type_list::push_front<tlist<double>, int>,
            tlist<double>::push_front<int>,
            tlist<int, double>
        >
    );

    static_assert(
        are_same_v<
            value_list::push_front<vlist<int>, 42>,
            vlist<int>::push_front<42>,
            vlist<int, 42>
        >
    );

    static_assert(
        are_same_v<
            value_list::push_front<vlist<int, 99>, 42>,
            vlist<int, 99>::push_front<42>,
            vlist<int, 42, 99>
        >
    );

    //
    // push_back
    //

   static_assert(
        are_same_v<
            type_list::push_back<tlist<>, int>,
            tlist<>::push_back<int>,
            tlist<int>
        >
    );

    static_assert(
        are_same_v<
            type_list::push_back<tlist<double>, int>,
            tlist<double>::push_back<int>,
            tlist<double, int>
        >
    );

    static_assert(
        are_same_v<
            value_list::push_back<vlist<int>, 42>,
            vlist<int>::push_back<42>,
            vlist<int, 42>
        >
    );

    static_assert(
        are_same_v<
            value_list::push_back<vlist<int, 99>, 42>,
            vlist<int, 99>::push_back<42>,
            vlist<int, 99, 42>
        >
    );

    //
    // concat
    //

    static_assert(
        are_same_v<
            type_list::concat<tlist<>, tlist<>>,
            tlist<>::concat<tlist<>>,
            tlist<>
        >
    );

    static_assert(
        are_same_v<
            type_list::concat<tlist<int>, tlist<>>,
            tlist<int>::concat<tlist<>>,
            tlist<int>
        >
    );

    static_assert(
        are_same_v<
            type_list::concat<tlist<>, tlist<int>>,
            tlist<>::concat<tlist<int>>,
            tlist<int>
        >
    );

    static_assert(
        are_same_v<
            type_list::concat<tlist<int, float>, tlist<double, short>>,
            tlist<int, float>::concat<tlist<double, short>>,
            tlist<int, float, double, short>
        >
    );

    static_assert(
        are_same_v<
            value_list::concat<vlist<int>, vlist<int>>,
            vlist<int>::concat<vlist<int>>,
            vlist<int>
        >
    );

    static_assert(
        are_same_v<
            value_list::concat<vlist<int, 1>, vlist<int>>,
            vlist<int, 1>::concat<vlist<int>>,
            vlist<int, 1>
        >
    );

    static_assert(
        are_same_v<
            value_list::concat<vlist<int>, vlist<int, 1>>,
            vlist<int>::concat<vlist<int, 1>>,
            vlist<int, 1>
        >
    );

    static_assert(
        are_same_v<
            value_list::concat<vlist<int, 1, 2>, vlist<int, 3, 4>>,
            vlist<int, 1, 2>::concat<vlist<int, 3, 4>>,
            vlist<int, 1, 2, 3, 4>
        >
    );

    //
    // reverse
    //

    static_assert(
        are_same_v<
            type_list::reverse<tlist<>>,
            tlist<>::reverse,
            tlist<>
        >
    );

    static_assert(
        are_same_v<
            type_list::reverse<tlist<int>>,
            tlist<int>::reverse,
            tlist<int>
        >
    );

    static_assert(
        are_same_v<
            type_list::reverse<tlist<int, float, short>>,
            tlist<int, float, short>::reverse,
            tlist<short, float, int>
        >
    );

    static_assert(
        are_same_v<
            value_list::reverse<vlist<int>>,
            vlist<int>::reverse,
            vlist<int>
        >
    );

    static_assert(
        are_same_v<
            value_list::reverse<vlist<int, 1>>,
            vlist<int, 1>::reverse,
            vlist<int, 1>
        >
    );

    static_assert(
        are_same_v<
            value_list::reverse<vlist<int, 1, 2, 3>>,
            vlist<int, 1, 2, 3>::reverse,
            vlist<int, 3, 2, 1>
        >
    );

    //
    // Transform
    //

    static_assert(
        are_same_v<
            type_list::transform<tlist<>, std::add_const>,
            tlist<>::transform<std::add_const>,
            tlist<>
        >
    );

    static_assert(
        are_same_v<
            type_list::transform<tlist<int, float, short>, std::add_const>,
            tlist<int, float, short>::transform<std::add_const>,
            tlist<const int, const float, const short>
        >
    );

    template<auto V>
    struct plus_one
    {
        static constexpr decltype(V) value = V + 1;
    };

    static_assert(
        are_same_v<
            value_list::transform<vlist<int>, plus_one>,
            vlist<int>::transform<plus_one>,
            vlist<int>
        >
    );

    static_assert(
        are_same_v<
            value_list::transform<vlist<int, 1, 2, 3>, plus_one>,
            vlist<int, 1, 2, 3>::transform<plus_one>,
            vlist<int, 2, 3, 4>
        >
    );

    //
    // get
    //

    static_assert(
        are_same_v<
            type_list::get<tlist<int>, 0>,
            tlist<int>::get<0>,
            int
        >
    );

    static_assert(
        are_same_v<
            type_list::get<tlist<int, short, float>, 1>,
            tlist<int, short, float>::get<1>,
            short
        >
    );

    static_assert(
        are_same_v<
            type_list::get<tlist<int, short, float>, 2>,
            tlist<int, short, float>::get<2>,
            float
        >
    );

    static_assert(
        are_equal_v<
            value_list::get<vlist<int, 42>, 0>,
            vlist<int, 42>::get<0>,
            42
        >
    );

    static_assert(
        are_equal_v<
            value_list::get<vlist<int, 41, 42, 43>, 1>,
            vlist<int, 41, 42, 43>::get<1>,
            42
        >
    );

    static_assert(
        are_equal_v<
            value_list::get<vlist<int, 41, 42, 43>, 2>,
            vlist<int, 41, 42, 43>::get<2>,
            43
        >
    );

    //
    // set
    //

    static_assert(
        are_same_v<
            type_list::set<tlist<int>, 0, float>,
            tlist<int>::set<0, float>,
            tlist<float>
        >
    );

    static_assert(
        are_same_v<
            type_list::set<tlist<int,float,short>, 1, double>,
            tlist<int, float, short>::set<1, double>,
            tlist<int, double, short>
        >
    );

    static_assert(
        are_same_v<
            type_list::set<tlist<int,float,short>, 2, double>,
            tlist<int,float,short>::set<2, double>,
            tlist<int,float,double>
        >
    );

    static_assert(
        are_same_v<
            value_list::set<vlist<int, 2>, 0, 42>,
            vlist<int, 2>::set<0, 42>,
            vlist<int, 42>
        >
    );

}