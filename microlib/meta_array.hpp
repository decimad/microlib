//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_META_HPP_
#define MICROLIB_META_HPP_

#include <cstddef>

namespace ulib
{
    namespace meta
    {
        namespace deprecated
        {
            using std::size_t;

            template <typename Type, Type... elements>
            struct array
            {
                using type = Type;
                static const unsigned int size = sizeof...(elements);
            };

            template <typename StaticArray, size_t index>
            struct get;

            template <typename Type, Type element0, Type... tail>
            struct get<array<Type, element0, tail...>, 0>
            {
                static const Type value = element0;
            };

            template <typename Type, Type element0, Type... tail, size_t index>
            struct get<array<Type, element0, tail...>, index>
            {
                static const Type value = get<array<Type, tail...>, index - 1>::value;
            };

            template <typename StaticArray, size_t index, typename StaticArray::type value,
                      typename NewArray = array<typename StaticArray::type>>
            struct set;

            template <size_t index, typename Type, Type src_element0, Type... src_tail, Type... dest_list,
                      typename array<Type, src_element0, src_tail...>::type value>
            struct set<array<Type, src_element0, src_tail...>, index, value, array<Type, dest_list...>>
            {
                using type = typename set<array<Type, src_tail...>, index - 1, value, array<Type, dest_list..., src_element0>>::type;
            };

            template <typename Type, Type src_element0, Type... src_tail, Type... dest_list,
                      typename array<Type, src_element0, src_tail...>::type value>
            struct set<array<Type, src_element0, src_tail...>, 0, value, array<Type, dest_list...>>
            {
                using type = array<Type, dest_list..., value, src_tail...>;
            };

        } // namespace deprecated
          /*

          template< typename Array, template< typename > class Function >
          struct transform;

          template< typename Type, Type... elements, template< Type > class Function >
          struct transform< static_array< Type, elements... >, Function >
          {
              using type = static_array< Type, Function< elements >::value... >;
          };

          */

    } // namespace meta
} // namespace ulib

#endif /* META_HPP_ */
