//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_STRING_HPP__
#define MICROLIB_STRING_HPP__

#include <cstring>

namespace ulib
{

    class string
    {
      public:
        string(char *data, size_t max_size);

        int printf(const char *fmt, ...);
        const char *data() const;
        const char *c_string() const;

        size_t size() const;
        size_t available() const;

        void clear();

        string &operator+=(const string &other);

      private:
        char *data_;
        size_t max_size_;
        size_t size_;
    };

    class const_string
    {
      public:
        const_string(const char *data) : data_(data), size_(strlen(data))
        {
        }

        template <unsigned int Size>
        const_string(char (&ref)[Size]) : data_(&ref), size_(Size)
        {
        }

        const char *c_string() const
        {
            return data_;
        }

        size_t size() const
        {
            return size_;
        }

      private:
        const char *data_;
        const size_t size_;
    };

    template <size_t Size>
    class static_string : public string
    {
      public:
        static_string() : string(data_, Size)
        {
        }

      private:
        char data_[Size];
    };

} // namespace ulib

#endif
