/*
 * meta.hpp
 *
 *  Created on: 24.11.2014
 *      Author: Michael
 */

#ifndef META_HPP_
#define META_HPP_

namespace meta {

	template< typename Type, Type... elements >
	struct static_array {
		using type = Type;
		static const unsigned int size = sizeof...(elements);
	};

	template< typename StaticArray, uint8 index >
	struct get;

	template< typename Type, Type element0, Type... tail >
	struct get< static_array< Type, element0, tail... >, 0 >
	{
		static const Type value = element0;
	};

	template< typename Type, Type element0, Type... tail, uint8 index >
	struct get< static_array< Type, element0, tail... >, index >
	{
		static const Type value = get< static_array< Type, tail... >, index-1 >::value;
	};

	template< typename StaticArray, uint8 index, typename StaticArray::type value, typename NewArray = static_array<typename StaticArray::type> >
	struct set;

	template< uint8 index, typename Type, Type value,  Type src_element0, Type... src_tail, Type... dest_list >
	struct set< static_array< Type, src_element0, src_tail... >, index, value, static_array< Type, dest_list... > >
	{
		using type = typename set< static_array< Type, src_tail... >, index-1, value, static_array< Type, dest_list..., src_element0 > >::type;
	};

	template< typename Type, Type value, Type src_element0, Type... src_tail, Type... dest_list >
	struct set< static_array< Type, src_element0, src_tail... >, 0, value, static_array< Type, dest_list... > >
	{
		using type = static_array< Type, dest_list..., value, src_tail... >;
	};

	/*

	template< typename Array, template< typename > class Function >
	struct transform;

	template< typename Type, Type... elements, template< Type > class Function >
	struct transform< static_array< Type, elements... >, Function >
	{
		using type = static_array< Type, Function< elements >::value... >;
	};

	*/

}



#endif /* META_HPP_ */
