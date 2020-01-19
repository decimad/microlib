//          Copyright Michael Steinberg 2016
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_META_LIST_HPP__
#define MICROLIB_META_LIST_HPP__

#include <cstddef>
#include <type_traits>


namespace ulib
{
    using std::size_t;

    namespace meta
    {

        struct error_type
        {
        };

        //
        // List utility
        //

        namespace detail
        {

            //
            // push_front
            //
            template <typename ListType, typename Arg>
            struct push_front;

            template <template <typename...> class ListType, typename Arg, typename... Args>
            struct push_front<ListType<Args...>, Arg>
            {
                using type = ListType<Arg, Args...>;
            };

            //
            // push_back
            //
            template <typename ListType, typename Arg>
            struct push_back;

            template <template <class...> class ListType, typename... Args, typename Arg>
            struct push_back<ListType<Args...>, Arg>
            {
                using type = ListType<Args..., Arg>;
            };

            //
            // reverse
            //
            template <typename List>
            struct reverse;

            template <template <class...> class ListType>
            struct reverse<ListType<>>
            {
                using type = ListType<>;
            };

            template <template <class...> class ListType, typename Element0, typename... Elements>
            struct reverse<ListType<Element0, Elements...>>
            {
                using type = typename push_back<typename reverse<ListType<Elements...>>::type, Element0>::type;
            };

            //
            // pop_front
            //
            template <typename ListType>
            struct pop_front;

            template <template <class...> class ListType, typename... Args, typename First>
            struct pop_front<ListType<First, Args...>>
            {
                using type = ListType<Args...>;
            };

            //
            // pop_front
            //
            template <typename ListType>
            struct pop_back;

            template <template <class...> class ListType, typename... Args>
            struct pop_back<ListType<Args...>>
            {
                using type = typename reverse<typename pop_front<typename reverse<ListType<Args...>>::type>::type>::type;
            };

        } // namespace detail

        //
        //
        //
        template <typename ListType>
        struct list_size;

        template <template <class...> class ListType, typename... Elems>
        struct list_size<ListType<Elems...>>
        {
            static constexpr size_t value = sizeof...(Elems);
        };

        template <typename ListType>
        constexpr size_t list_size_v = list_size<ListType>::value;

        //
        //
        //
        template <typename T>
        struct is_empty
        {
            static constexpr bool value = true;
        };

        template <template <typename...> class List, typename First, typename... Tail>
        struct is_empty<List<First, Tail...>>
        {
            static constexpr bool value = false;
        };

        template <typename List>
        constexpr bool is_empty_v = is_empty<List>::value;

        // reverse_t< List >
        // reverse the elements of a type_list
        template <typename List>
        using reverse_t = typename detail::reverse<List>::type;

        // push_front_t< List, Arg >
        // add new element Arg to the front of the type_list List
        template <typename List, typename Arg>
        using push_front_t = typename detail::push_front<List, Arg>::type;

        // pop_front< List >
        // remove first element of type_list List (if the list is empty, the result is error_type)
        template <typename List>
        using pop_front_t = typename detail::pop_front<List>::type;

        // push_back_t< List, Arg >
        // add new element Arg to the ent of the type_list List
        template <typename List, typename Arg>
        using push_back_t = typename detail::push_back<List, Arg>::type;

        // pop_back_t< List >
        // remove last element of the type_list List (if the list is empty, the result is error_type)
        template <typename List>
        using pop_back_t = typename detail::pop_back<List>::type;

        // replace_front_t<List, Elem>
        template <typename List, typename Elem>
        using replace_front_t = push_front_t<pop_front_t<List>, Elem>;

        // replace_back_t<List, Elem>
        template <typename List, typename Elem>
        using replace_back_t = push_back_t<pop_back_t<List>, Elem>;

        // front< List >
        //
        namespace detail
        {

            template <typename List>
            struct front
            {
                using type = error_type;
            };

            template <template <class...> class ListType, typename Front, typename... Tail>
            struct front<ListType<Front, Tail...>>
            {
                using type = Front;
            };

        } // namespace detail

        template <typename List>
        using front_t = typename detail::front<List>::type;

        // back< List >
        template <typename List>
        using back_t = front_t<reverse_t<List>>;

        // join< List1, List2 >
        namespace detail
        {

            template <typename List1, typename List2>
            struct join_list_struct;

            template <template <class...> class ListType, typename... Elems1, typename... Elems2>
            struct join_list_struct<ListType<Elems1...>, ListType<Elems2...>>
            {
                using type = ListType<Elems1..., Elems2...>;
            };

        } // namespace detail

        template <typename List1, typename List2>
        using join_t = typename detail::join_list_struct<List1, List2>::type;

        namespace detail
        {

            template <typename List, template <typename> class Transformer>
            struct transform_list_struct;

            template <template <typename...> class ListType, typename Arg0, typename... Tail, template <typename> class Transformer>
            struct transform_list_struct<ListType<Arg0, Tail...>, Transformer>
            {
                using type
                    = push_front_t<typename transform_list_struct<ListType<Tail...>, Transformer>::type, typename Transformer<Arg0>::type>;
            };

            template <template <typename...> class ListType, template <typename> class Transformer>
            struct transform_list_struct<ListType<>, Transformer>
            {
                using type = ListType<>;
            };

        } // namespace detail

        template <typename List, template <typename> class Transformer>
        using transform_t = typename detail::transform_list_struct<List, Transformer>::type;

        // clear
        //
        //
        template <typename T>
        struct clear;

        template <template <typename...> class ListType, typename... Args>
        struct clear<ListType<Args...>>
        {
            using type = ListType<>;
        };

        template <typename T>
        using clear_t = typename clear<T>::type;

        // Declare are basic type list that is compatible with the Range concept
        //
        //
        template <typename... Types>
        struct list_iterator
        {
        };

        template <typename Type0, typename... Types>
        struct list_iterator<Type0, Types...>
        {
            using advance = list_iterator<Types...>;
            using dereference = Type0;
        };

        template <typename... Elems>
        struct list
        {
            struct end_type
            {
            };

            using begin = end_type;
            using end = end_type;

            static constexpr bool empty()
            {
                return true;
            }
        };

        template <typename Elem0, typename... Elems>
        struct list<Elem0, Elems...>
        {
            using begin = list_iterator<Elems...>;
            using end = list_iterator<>;

            using advance = pop_front_t<list>;
            using dereference = front_t<list>;

            static constexpr bool empty()
            {
                return false;
            }
        };

        template <typename Type, Type... Elements>
        using integral_list = list<std::integral_constant<Type, Elements>...>;

        // MSVC doesn't currently support template member aliases as means to binding arguments in "functors",
        // that's why we support passing in additional arguments to erase_if_t
        namespace detail
        {

            template <typename List, template <typename...> class Functor, typename Result = clear_t<List>,
                      bool IsEmpty = is_empty<List>::value>
            struct erase_if
            {
                using type = Result;
            };

            template <typename List, template <typename> class Functor, typename Result>
            struct erase_if<List, Functor, Result, false>
            {
                using type =
                    typename erase_if<pop_front_t<List>, Functor,
                                      std::conditional_t<Functor<front_t<List>>::value, Result, push_back_t<Result, front_t<List>>>>::type;
            };

        } // namespace detail

        template <typename TypeList, template <typename> class Functor>
        using erase_if_t = typename detail::erase_if<TypeList, Functor>::type;

        // concat - concatenate type lists
        //
        //
        template <typename... Lists>
        struct concat
        {
        };

        template <template <typename...> class ListType, typename... Args1, typename... Args2, typename... Tail>
        struct concat<ListType<Args1...>, ListType<Args2...>, Tail...>
        {
            using type = typename concat<ListType<Args1..., Args2...>, Tail...>::type;
        };

        template <typename List>
        struct concat<List>
        {
            using type = List;
        };

        template <typename... Lists>
        using concat_t = typename concat<Lists...>::type;

        // find_index
        //
        //
        template <typename List, typename Type, size_t Index = 0, bool Here = std::is_same<front_t<List>, Type>::value>
        struct find_index
        {
            static constexpr size_t value = find_index<pop_front_t<List>, Type, Index + 1>::value;
        };

        template <typename List, typename Type, size_t Index>
        struct find_index<List, Type, Index, true>
        {
            static constexpr size_t value = Index;
        };

        template <typename List, typename Type>
        constexpr size_t find_index_v = find_index<List, Type>::value;

        // for_each
        //
        //
        template <typename List, template <typename, typename> class Body, typename State>
        struct for_each
        {
            using type = typename for_each<pop_front_t<List>, Body, typename Body<front_t<List>, State>::type>::type;
        };

        template <template <typename...> class ListType, template <typename, typename> class Body, typename State>
        struct for_each<ListType<>, Body, State>
        {
            using type = State;
        };

        template <typename List, template <typename, typename> class Body, typename State>
        using for_each_t = typename for_each<List, Body, State>::type;

        // get_index
        //
        //
        template <typename List, size_t Index>
        struct get
        {
            using type = typename get<pop_front_t<List>, Index - 1>::type;
        };

        template <typename List>
        struct get<List, 0>
        {
            using type = front_t<List>;
        };

        template <typename List, size_t Index>
        using get_t = typename get<List, Index>::type;

        // replace
        //
        //
        /*
            template< typename List, size_t Index, typename Type >
            struct replace;

            namespace detail {
                template< template< typename... > class ListType, typename SrcList, size_t Index, typename FrontList = ListType<> >
                struct replace_impl;

                template< template< typename... > class ListType, typename SrcElement0, typename... SrcElements, size_t Index, typename
           FrontList > struct replace_impl< ListType< SrcElement0, SrcElements... >, size_t Index, FrontList >
                {

                }
            }

            template< template< typename... > class ListType, typename... Elements, size_t Index, typename Type >
            struct replace< ListType<Elements...>, Index, Type >
            {

            };
        */

    } // namespace meta
} // namespace ulib

#endif
