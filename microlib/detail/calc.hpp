/*
 * calc.hpp
 *
 *  Created on: 04.11.2016
 *      Author: Michael
 */


namespace ulib {

namespace detail {

	template< typename T >
	constexpr bool is_power_of_2( T val )
	{
		return (val > 1) ? is_power_of_2( val/T(2) ) : (val == T(1));
	}

}

}

