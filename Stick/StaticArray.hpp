#ifndef STICK_STATIC_ARRAY_HPP
#define STICK_STATIC_ARRAY_HPP

#include <Stick/Iterator.hpp>

namespace stick
{
    template<class T, Size C>
    class StaticArray
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
            return (Iter)m_array + m_count;
        }

        inline ConstIter end() const
        {
            return (ConstIter)m_array + m_count;
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

        inline Size count() const
        {
            return m_count;
        }

        inline constexpr Size capacity() const
        {
            return C;
        }

        template<class D>
        inline bool append(D && _item)
        {
            if(m_count == C)
                return false;
            m_array[m_count++] = std::forward<D>(_item);
            return true;
        }

        T m_array[C ? C : 1];
        Size m_count;
    };
};

#endif //STICK_STATIC_ARRAY_HPP
