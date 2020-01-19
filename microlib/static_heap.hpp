//          Copyright Michael Steinberg 2015
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MICROLIB_STATIC_HEAP_HPP__
#define MICROLIB_STATIC_HEAP_HPP__

#include <microlib/static_vector.hpp>
#include <microlib/util.hpp>

namespace ulib
{

    //
    // Since std::heap stuff does not support random erasing, which is a must have, we have to implement
    // sift operations, thus we can implement the whole stuff...
    //

    // heap with static capacity of Size
    // stored types T must support move/copy construction and move/copy assignment.
    template <typename T, size_t Size, typename Compare = std::less<T>>
    struct static_heap : public detail::ebo<Compare>
    {
      public:
        using container_type = static_vector<T, Size + 1, 1>;
        using iterator = typename container_type::iterator;
        using const_iterator = typename container_type::const_iterator;

        using size_type = typename container_type::size_type;

        static_heap(Compare compare = Compare()) : detail::ebo<Compare>(std::move(compare))
        {
        }

        // Inserts a value into the heap
        // Returns false iff there is not enough storage capacity left.
        template <typename ValType>
        bool push(ValType &&val)
        {
            return push_back_and_sift_up(std::forward<ValType>(val));
        }

        // Inserts a value into the heap, constructed with exactly
        // the given arguments. If the heap is empty, no moves
        // are involved.
        // Returns false iff there is not enough storage capacity left.
        template <typename... Args>
        bool emplace(Args &&... args)
        {
            return emplace_back_and_sift_up(std::forward<Args>(args)...);
        }

        // Replaces the current top value with the given value, restoring
        // the heap invariants.
        template <typename ValType>
        void replace(ValType &&val)
        {
            sift_up_move(sift_down_hole(root()), std::forward<ValType>(val));
        }

        // Restores the heap invariants when they were broken by changing
        // the top value.
        void restore()
        {
            sift_down_inplace(root());
        }

        // Removes the current top value, restoring the heap invariants.
        // UB if the heap is empty.
        void pop()
        {
            auto pos = sift_down_hole(root());
            if (pos != data_.size())
            {
                sift_up_move(pos, std::move(data_[data_.size()]));
            }
            data_.pop_back();
        }

        // Returns a reference to the current top value.
        // UB if the heap is empty.
        T &top_element()
        {
            return data_[root()];
        }

        // Returns a const reference to the current top value.
        // UB if the heap is empty.
        const T &top_element() const
        {
            return data_[root()];
        }

        // Returns an iterator to the first element of the heap
        // or one past the end, if the heap is empty.
        iterator begin()
        {
            return data_.begin();
        }

        // Returns an iterator to the element one past the last.
        iterator end()
        {
            return data_.end();
        }

        const_iterator begin() const
        {
            return data_.begin();
        }

        const_iterator end() const
        {
            return data_.end();
        }

        // Fixes the heap invariants after an element pointed
        // to by the given iterator has been changed.
        void restore(const_iterator it)
        {
            sift_fix(it - begin() + root());
        }

        // Returns the current element count.
        size_type size() const
        {
            return data_.size();
        }

        // Returns the maximum element count.
        size_type capacity() const
        {
            return data_.capacity();
        }

        void erase(const_iterator it)
        {
            const size_type idx = it - begin();
            auto hole = sift_down_hole(idx);
            if (hole != data_.size())
            {
                sift_up_move(hole, std::move(data_[data_.size()]));
            }
            data_.pop_back();
        }

      private:
        size_type sift_up_inplace(size_type index)
        {
            return sift_up(index, std::move(data_[index]));
        }

        size_type sift_up(size_type hole, T val)
        {
            while (hole > root() && !compare(data_[parent(hole)], val))
            {
                data_[hole] = std::move(data_[parent(hole)]);
                hole = parent(hole);
            }

            data_[hole] = std::move(val);
            return hole;
        }

        size_type sift_up_move(size_type hole, T &&val)
        {
            while (hole > root() && !compare(data_[parent(hole)], val))
            {
                data_[hole] = std::move(data_[parent(hole)]);
                hole = parent(hole);
            }

            data_[hole] = std::move(val);
            return hole;
        }

        bool push_back_and_sift_up(T &&val)
        {
            if (data_.size() != data_.capacity())
            {
                // since we want to allow types which are not
                // default-constructible, we need to know
                // what belongs into the appended spot.
                size_type index = parent(data_.size());
                if (index != 0 && !compare(data_[index], val))
                {
                    data_.emplace_back(std::move(data_[index]));
                    sift_up_move(index, std::move(val));
                }
                else
                {
                    data_.emplace_back(std::move(val));
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        template <typename... Args>
        bool emplace_back_and_sift_up(Args &&... args)
        {
            if (!data_.size())
            {
                data_.emplace_back(std::forward<Args>(args)...);
                return true;
            }
            else
            {
                return push_back_and_sift_up(T(std::forward<Args>(args)...));
            }
        }

        // returns true if the sift_up_operation had an effect
        // used by sift_fix only
        bool sift_up_inplace_checked(size_type index)
        {
            return sift_up_inplace(index) != index;
        }

        // sifts down a value that is already resident in the heap
        void sift_down_inplace(size_type index)
        {
            T val = std::move(data_[index]);
            sift_up_move(sift_down_hole(index), std::move(val));
        }

        // sifts down a hole down to a leaf, returns final hole index
        size_type sift_down_hole(size_type hole)
        {
            const auto s = data_.size();
            while (lhs(hole) <= s)
            {
                if (rhs(hole) <= s && compare(data_[rhs(hole)], data_[lhs(hole)]))
                {
                    data_[hole] = data_[rhs(hole)];
                    hole = rhs(hole);
                }
                else
                {
                    data_[hole] = data_[lhs(hole)];
                    hole = lhs(hole);
                }
            }
            return hole;
        }

        // sifts down an external value
        void sift_down(size_type hole, T val)
        {
            const auto s = data_.size();
            while (lhs(hole) <= s)
            {
                auto next_child = lhs(hole);
                if (rhs(hole) <= s && !compare(data_[lhs(hole)], data_[rhs(hole)]))
                    next_child = rhs(hole);
                if (compare(val, data_[next_child]))
                    break;

                data_[hole] = std::move(data_[next_child]);
                hole = next_child;
            }
            data_[hole] = std::move(val);
        }

        // fixes the position of a node that was changed from outside
        void sift_fix(size_type i)
        {
            if (!sift_up_inplace_checked(i))
            {
                sift_down_inplace(i);
            }
        }

        bool compare(const T &a, const T &b)
        {
            return static_cast<Compare *>(this)->operator()(a, b);
        }

        static constexpr size_type parent(size_type idx)
        {
            return idx / 2;
        }

        static constexpr size_type lhs(size_type idx)
        {
            return idx * 2;
        }

        static constexpr size_type rhs(size_type idx)
        {
            return idx * 2 + 1;
        }

        static constexpr size_type root()
        {
            return 1;
        }

      private:
        container_type data_;

#ifdef _DEBUG
      public:
        bool equals(const T &a, const T &b)
        {
            return !compare(a, b) && !compare(b, a);
        }

        bool less_equal(const T &a, const T &b)
        {
            return compare(a, b) || equals(a, b);
        }

        bool test_invariant()
        {
            for (size_type i = root(); i <= data_.size(); ++i)
            {
                if (lhs(i) <= data_.size() && !less_equal(data_[i], data_[lhs(i)]))
                {
                    return false;
                }
                if (rhs(i) <= data_.size() && !less_equal(data_[i], data_[rhs(i)]))
                {
                    return false;
                }
            }
            return true;
        }
#endif
    };

} // namespace ulib

#endif
