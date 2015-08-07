#ifndef STM_UTIL_UTILSHARED_HPP__
#define STM_UTIL_UTILSHARED_HPP__

namespace util {

	namespace detail {

		// Simple empty-base-class optimizer.

		template< typename... Bases >
		struct ebo : public Bases... {
			ebo(Bases... args)
				: Bases(std::move(args))...
			{}
		};

	}

}

#endif
