#ifndef STICK_FIXED_ARRAY_HPP
#define STICK_FIXED_ARRAY_HPP

#include <Stick/Iterator.hpp>

namespace stick
{
    template<class T, Size C>
    class FixedArray
    {
    public:

        typedef T ValueType;

        typedef T * Iter;

        typedef const T * ConstIter;

        typedef ReverseIterator<Iter> ReverseIter;

        typedef ReverseIterator<ConstIter> ReverseConstIter;


        inline const T & operator [](Size _index) const
        {
            return m_array[_index];
        }

        inline T & operator [](Size _index)
        {
            return m_array[_index];
        }

        inline Iter begin()
        {
            return (Iter)m_array;
        }

        inline ConstIter begin() const
        {
            return (ConstIter)m_array;
        }

        inline Iter end()
        {
            return (Iter)m_array + C;
        }

        inline ConstIter end() const
        {
            return (ConstIter)m_array + C;
        }

        inline ReverseIter rbegin()
        {
            return ReverseIter(end() - 1);
        }

        inline ReverseConstIter rbegin() const
        {
            return ReverseConstIter(end() - 1);
        }

        inline ReverseIter rend()
        {
            return ReverseIter(begin());
        }

        inline ReverseConstIter rend() const
        {
            return ReverseConstIter(begin());
        }

        inline constexpr Size count() const
        {
            return C;
        }

        inline const T * ptr() const
        {
            return &m_array[0];
        }

        inline T * ptr()
        {
            return &m_array[0];
        }

        T m_array[C ? C : 1];
    };
};

#endif //STICK_FIXED_ARRAY_HPP
