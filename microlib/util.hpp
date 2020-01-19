//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_UTIL_HPP__
#define MICROLIB_UTIL_HPP__
#include <cstddef>
#include <type_traits>
#include <utility>


namespace ulib
{

    namespace detail
    {

        // Simple empty-base-class optimizer.

        template <typename... Bases>
        struct ebo : public Bases...
        {
            ebo(Bases... args) : Bases(std::move(args))...
            {
            }
        };

    } // namespace detail

    template <bool Cond, typename A, typename B>
    using conditional_t = typename std::conditional<Cond, A, B>::type;

    template <bool Cond, typename T = void>
    using enable_if_t = typename std::enable_if<Cond, T>::type;

    template <typename... Types>
    struct max_alignment;

    template <typename Type0, typename... Types>
    struct max_alignment<Type0, Types...>
    {
        static const size_t value = (std::alignment_of<Type0>::value > max_alignment<Types...>::value) ? std::alignment_of<Type0>::value
                                                                                                       : max_alignment<Types...>::value;
    };

    template <>
    struct max_alignment<>
    {
        static const size_t value = 0;
    };

    template <typename... Types>
    struct max_size;

    template <typename Type0, typename... Types>
    struct max_size<Type0, Types...>
    {
        static const size_t value = (sizeof(Type0) > max_size<Types...>::value) ? sizeof(Type0) : max_size<Types...>::value;
    };

    template <>
    struct max_size<>
    {
        static const size_t value = 0;
    };

    template <typename T>
    constexpr T max(T a, T b)
    {
        return (a > b) ? a : b;
    }

    template <typename T>
    constexpr T min(T a, T b)
    {
        return (a < b) ? a : b;
    }

    template <typename T>
    constexpr T abs(T value)
    {
        return (value >= 0) ? value : -value;
    }

} // namespace ulib

#endif
