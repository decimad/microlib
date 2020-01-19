//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_STATEMACHINE_HPP__
#define MICROLIB_STATEMACHINE_HPP__

#include <microlib/variant.hpp>

namespace ulib
{

    template <typename... States>
    struct state_machine
    {

        state_machine()
        {
        }

        template <typename ToState, typename... Args>
        void to_state(Args &&... args)
        {
            storage_.template to_type<ToState>(std::forward<Args>(args)...);
        }

        template <typename Iface>
        Iface *get_state_interface()
        {
            return storage_.template get_interface<Iface>();
        }

        template <typename T>
        bool is_state() const
        {
            return storage_.template is<T>();
        }

        template <typename T>
        T &as_state()
        {
            return storage_.template as<T>();
        }

        template <typename T>
        const T &as_state() const
        {
            return storage_.template as<T>();
        }

        template <typename... Cases, typename... Args>
        void dispatch(Args &&... args)
        {
            storage_.template dispatch<Cases...>(std::forward<Args>(args)...);
        }

        template <typename... Cases, typename... Args>
        void dispatch_self(Args &&... args)
        {
            storage_.template dispatch_self<Cases...>(std::forward<Args>(args)...);
        }

      private:
        variant<States...> storage_;
    };

} // namespace ulib

#endif
