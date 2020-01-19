//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_STATIC_INTERVAL_HEAP_HPP__
#define MICROLIB_STATIC_INTERVAL_HEAP_HPP__

#include <functional>
#include <microlib/static_vector.hpp>
#include <microlib/util.hpp>


namespace ulib
{

    template <typename T, size_t Size, typename Compare = std::less<T>>
    class static_inverval_heap : public ulib::detail::ebo<Compare>
    {
      public:
        using storage_type = static_vector<T, Size + 2, 2>;
        using size_type = typename storage_type::size_type;

        using iterator = typename storage_type::iterator;
        using const_iterator = typename storage_type::const_iterator;

        static_inverval_heap(Compare comp = Compare()) : ulib::detail::ebo<Compare>(std::move(comp))
        {
        }

      public:
        const T &min_element() const
        {
            return data_[2];
        }

        T &min_element()
        {
            return const_cast<T &>(static_cast<const static_inverval_heap *>(this)->min_element());
        }

        const T &max_element() const
        {
            if (size() > 1)
            {
                return data_[3];
            }
            else
            {
                return data_[2];
            }
        }

        T &max_element()
        {
            return const_cast<T &>(static_cast<const static_inverval_heap *>(this)->max_element());
        }

        size_type size() const
        {
            return data_.size();
        }

        size_type capacity() const
        {
            return data_.capacity();
        }

        void pop_min()
        {
            auto pos = sift_down_hole_min(1);
            if (index(pos, 0) != (data_.size() + 1))
            {
                sift_up(pos, 0, std::move(data_[data_.size() + 1]));
            }
            data_.pop_back();
        }

        void pop_max()
        {
            if (size() == 1)
            {
                data_.pop_back();
            }
            else
            {
                auto pos = sift_down_hole_max(1);
                if (index(pos, 1) != (data_.size() + 1))
                {
                    sift_up(pos, 1, std::move(data_[data_.size() + 1]));
                }
                data_.pop_back();
            }
        }

        // replace max or min respectively
        template <typename ValType>
        void replace_max(ValType &&val)
        {
            sift_up(sift_down_hole_max(1), 1, std::forward<ValType>(val));
        }

        template <typename ValType>
        void replace_min(ValType &&val)
        {
            sift_up(sift_down_hole_min(1), 0, std::forward<ValType>(val));
        }

        template <typename ValType>
        bool push(ValType &&val)
        {
            // This is more complicated than it needs to be, beca�se
            // we're trying hard to avoid to default-initialize the
            // new element inside the array.

            if (!data_.size())
            {
                data_.emplace_back(std::forward<ValType>(val));
                return true;
            }
            else if (data_.size() != data_.capacity())
            {
                const auto vindex = (data_.size() + 2) / 2;
                const unsigned char side = data_.size() % 2;

                if (side == 1)
                {
                    if (!compare(data_[index(vindex, 0)], val))
                    {
                        // move left to right
                        data_.emplace_back(std::move(element(vindex, 0)));
                        sift_up(vindex, 0, std::forward<ValType>(val));
                    }
                    else
                    {
                        // hole was in right side (extending an existing node)
                        if (vindex != 1 && compare(element(parent(vindex), 1), val))
                        {
                            data_.emplace_back(std::move(element(parent(vindex), 1)));
                            sift_up(parent(vindex), 1, std::forward<ValType>(val));
                        }
                        else
                        {
                            data_.emplace_back(std::forward<ValType>(val));
                        }
                    }
                }
                else
                {
                    // hole in left side (we're creating a new node)
                    // side == 0
                    // since size was not 0, we must have a parent
                    if (!compare(element(parent(vindex), 0), val))
                    {
                        data_.emplace_back(std::move(element(parent(vindex), 0)));
                        sift_up(parent(vindex), 0, std::forward<ValType>(val));
                    }
                    else if (compare(element(parent(vindex), 1), val))
                    {
                        data_.emplace_back(std::move(element(parent(vindex), 1)));
                        sift_up(parent(vindex), 1, std::forward<ValType>(val));
                    }
                    else
                    {
                        data_.emplace_back(std::forward<ValType>(val));
                    }
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        template <typename... Args>
        bool emplace(Args &&... args)
        {
            if (!data_.size())
            {
                data_.emplace_back(std::forward<Args>(args)...);
            }
            else if (data_.size() != data_.capacity())
            {
                return push(T(std::forward<Args>(args)...));
            }
        }

        // iterator interface
      public:
        iterator begin()
        {
            return data_.begin();
        }

        const_iterator begin() const
        {
            return data_.begin();
        }

        iterator end()
        {
            return data_.end();
        }

        const_iterator end() const
        {
            return data_.end();
        }

        iterator restore(iterator where)
        {
            size_type idx = where - begin() + 2;
            unsigned char side = idx % 2;
            T val(std::move(data_[idx]));
            idx = sift_up(sift_down_hole(idx / 2, side), side, std::move(val));
            return begin() + (idx - 2);
        }

        void erase(iterator where)
        {
            size_type idx = where - begin() + 2;
            unsigned char side = idx % 2;

            idx = sift_down_hole(idx / 2, side);

            if (index(idx, side) != data_.size() + 1)
            {
                sift_up(idx, side, std::move(data_.size() + 1));
            }

            data_.pop_back();
        }

      private:
        size_type sift_down_hole_min(size_type vindex)
        {
            const auto s = size() + 2;
            while (index(lhs(vindex), 0) < s)
            {
                if (index(rhs(vindex), 0) < s && !compare(element(lhs(vindex), 0), element(rhs(vindex), 0)))
                {
                    element(vindex, 0) = std::move(element(rhs(vindex), 0));
                    vindex = rhs(vindex);
                }
                else
                {
                    element(vindex, 0) = std::move(element(lhs(vindex), 0));
                    vindex = lhs(vindex);
                }
            }

            return vindex;
        }

        size_type sift_down_hole_max(size_type vindex)
        {
            const auto s = size() + 2;
            while (index(lhs(vindex), 1) < s)
            {
                if (index(rhs(vindex), 1) < s && compare(element(lhs(vindex), 1), element(rhs(vindex), 1)))
                {
                    element(vindex, 1) = std::move(element(rhs(vindex), 1));
                    vindex = rhs(vindex);
                }
                else
                {
                    element(vindex, 1) = std::move(element(lhs(vindex), 1));
                    vindex = lhs(vindex);
                }
            }

            return vindex;
        }

        size_type sift_down_hole(size_type vindex, unsigned char side)
        {
            if (side == 0)
            {
                return sift_down_hole_min(vindex);
            }
            else
            {
                return sift_down_hole_max(vindex);
            }
        }

        size_type sift_up(size_type vindex, unsigned char side, T value)
        {
            if (!invariant(value, element(vindex, side ^ 1), side))
            {
                element(vindex, side) = std::move(element(vindex, side ^ 1));
                side = side ^ 1;
            }

            while (vindex != 1 && !invariant(element(parent(vindex), side), value, side))
            {
                element(vindex, side) = std::move(element(parent(vindex), side));
                vindex = parent(vindex);

                if (!invariant(value, element(vindex, side ^ 1), side))
                {
                    element(vindex, side) = std::move(element(vindex, side ^ 1));
                    side = side ^ 1;
                }
            }

            element(vindex, side) = std::move(value);
            return index(vindex, side);
        }

      private:
        // real indices * 2
        static size_type parent(size_type vindex)
        {
            return vindex / 2;
        }

        static size_type lhs(size_type vindex)
        {
            return vindex * 2;
        }

        static size_type rhs(size_type vindex)
        {
            return (vindex * 2) + 1;
        }

        static size_type index(size_type virtual_index, unsigned char side)
        {
            return virtual_index * 2 + side;
        }

        T &element(size_type virtual_index, unsigned char side)
        {
            return data_[index(virtual_index, side)];
        }

        const T &element(size_type virtual_index, unsigned char side) const
        {
            return data_[index(virtual_index, side)];
        }

        bool compare(const T &a, const T &b)
        {
            return static_cast<Compare *>(this)->operator()(a, b);
        }

        bool invariant(const T &left, const T &right, unsigned char side)
        {
            return (compare(left, right) ? 0 : 1) == side;
        }

      private:
        storage_type data_;
    };

} // namespace ulib

#endif
