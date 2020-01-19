//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <microlib/string.hpp>


namespace ulib
{

    string::string(char *data, size_t max_size) : data_(data), max_size_(max_size), size_(0)
    {
    }

    int string::printf(const char *fmt, ...)
    {
        int ret;
        va_list myargs;
        va_start(myargs, fmt);
        size_t avail = available();

        ret = vsnprintf(data_, avail, fmt, myargs);
        if (ret >= 0 && size_t(ret) <= avail)
        {
            size_ += ret;
        }
        va_end(myargs);
        return ret;
    }

    void string::clear()
    {
        size_ = 0;
    }

    size_t string::size() const
    {
        return size_;
    }

    size_t string::available() const
    {
        return max_size_ - size_ - 1;
    }

    string &string::operator+=(const string &other)
    {
        size_t othersize = other.size();

        if (othersize <= available())
        {
            memcpy(data_ + size_, other.data(), othersize);
            size_ += othersize;
        }

        return *this;
    }

    const char *string::c_string() const
    {
        data_[size_] = 0;
        return data_;
    }

    const char *string::data() const
    {
        return data_;
    }

} // namespace ulib
