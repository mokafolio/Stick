#ifndef STICK_STATIC_ARRAY_HPP
#define STICK_STATIC_ARRAY_HPP

#include <Stick/Iterator.hpp>
#include <initializer_list>

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

        inline StaticArray() :
            m_count(0)
        {

        }

        inline StaticArray(std::initializer_list<T> _items) :
        m_count(_items.size())
        {
            //@TODO: Static Assert on c++14+ where std::initializer_list<T>::size() is constexpr.
            // static_assert(_items.size() <= C, "Too many items for the capacity of this StaticArray");
            STICK_ASSERT(_items.size() <= C);
            std::copy(_items.begin(), _items.end(), &m_array[0]);
        }

        inline StaticArray(const StaticArray &) = default;
        inline StaticArray(StaticArray &&) = default;

        inline const T & operator [](Size _index) const
        {
            STICK_ASSERT(_index < m_count);
            return m_array[_index];
        }

        inline T & operator [](Size _index)
        {
            STICK_ASSERT(_index < m_count);
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
            if (m_count == C)
                return false;
            m_array[m_count++] = std::forward<D>(_item);
            return true;
        }

        inline void removeLast()
        {
            //@TODO: Do we need to call the destructor here?
            //or do we just let it get called once the array entry
            //might get overwritten?
            --m_count;
        }

    private:

        T m_array[C ? C : 1];
        Size m_count;
    };
};

#endif //STICK_STATIC_ARRAY_HPP
