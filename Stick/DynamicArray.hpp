#ifndef STICK_DYNAMICARRAY_HPP
#define STICK_DYNAMICARRAY_HPP

#include <Stick/Allocator.hpp>
#include <Stick/Iterator.hpp>
#include <Stick/Utility.hpp>
#include <initializer_list>
#include <new>

namespace stick
{
    //TODO: Make this append etc. work for types that are not default constructible/copy constructible
    //i.e. with the appropriate move versions.
    template<class T>
    class DynamicArray
    {
    public:

        typedef T ValueType;

        typedef T * Iter;

        typedef const T * ConstIter;

        typedef ReverseIterator<Iter> ReverseIter;

        typedef ReverseIterator<ConstIter> ReverseConstIter;


        DynamicArray(Allocator & _alloc = defaultAllocator()) :
            m_count(0),
            m_allocator(&_alloc)
        {

        }

        DynamicArray(std::initializer_list<T> _il) :
            m_count(0),
            m_allocator(&defaultAllocator())
        {
            insert(end(), _il.begin(), _il.end());
        }

        DynamicArray(Size _size, Allocator & _alloc = defaultAllocator()) :
            m_count(0),
            m_allocator(&_alloc)
        {
            resize(_size);
        }

        DynamicArray(const DynamicArray & _other) :
            m_count(0),
            m_allocator(_other.m_allocator)
        {
            if (_other.m_count)
            {
                insert(end(), _other.begin(), _other.end());
            }
        }

        DynamicArray(DynamicArray && _other) :
            m_data(std::move(_other.m_data)),
            m_count(std::move(_other.m_count)),
            m_allocator(std::move(_other.m_allocator))
        {
            //we don't want other to deallocate anything
            _other.m_data.ptr = nullptr;
        }

        ~DynamicArray()
        {
            deallocate();
        }

        inline DynamicArray & operator = (const DynamicArray & _other)
        {
            deallocate();
            m_count = 0;
            m_allocator = _other.m_allocator;
            insert(end(), _other.begin(), _other.end());

            return *this;
        }

        inline DynamicArray & operator = (std::initializer_list<T> _il)
        {
            clear();
            insert(end(), _il.begin(), _il.end());
            return *this;
        }

        inline DynamicArray & operator = (DynamicArray && _other)
        {
            deallocate();
            m_data = std::move(_other.m_data);
            m_allocator = std::move(_other.m_allocator);
            m_count = std::move(_other.m_count);
            _other.m_data.ptr = nullptr;

            return *this;
        }

        inline void resize(Size _s)
        {
            Size oldSize = m_count;
            reserve(_s);
            m_count = _s;
            if (oldSize < m_count)
            {
                for (Size i = oldSize; i < m_count; ++i)
                {
                    new (reinterpret_cast<T *>(m_data.ptr) + i) T();
                }
            }
        }

        inline void resize(Size _s, const T & _initial)
        {
            Size oldSize = m_count;
            reserve(_s);
            m_count = _s;
            if (oldSize < m_count)
            {
                for (Size i = oldSize; i < m_count; ++i)
                {
                    new (reinterpret_cast<T *>(m_data.ptr) + i) T(_initial);
                }
            }
        }

        inline void reserve(Size _s)
        {
            auto c = capacity();
            if (_s > c)
            {
                STICK_ASSERT(m_allocator);
                auto blk = m_allocator->allocate(_s * sizeof(T), alignof(T));
                STICK_ASSERT(blk);
                T * arrayPtr = reinterpret_cast<T *>(blk.ptr);
                T * sourcePtr = reinterpret_cast<T *>(m_data.ptr);

                //move the existing elements over
                for (Size i = 0; i < m_count; ++i)
                {
                    new (arrayPtr + i) T(std::move(sourcePtr[i]));
                }

                //call the destructors on the old elements
                for (Size i = 0; i < m_count; ++i)
                {
                    sourcePtr[i].~T();
                }

                if (m_data)
                    m_allocator->deallocate(m_data);

                m_data = blk;
            }
        }

        inline void append(std::initializer_list<T> _l)
        {
            insert(end(), _l.begin(), _l.end());
        }

        inline void append(const T & _element)
        {
            if (capacity() <= m_count)
            {
                reserve(max((Size)1, m_count * 2));
            }
            new (reinterpret_cast<T *>(m_data.ptr) + m_count++) T(_element);
        }

        inline void append(T && _element)
        {
            if (capacity() <= m_count)
            {
                reserve(max((Size)1, m_count * 2));
            }
            new (reinterpret_cast<T *>(m_data.ptr) + m_count++) T(std::move(_element));
        }

        template<class InputIter>
        inline void append(InputIter _first, InputIter _last)
        {
            insert(end(), _first, _last);
        }

        template<class InputIter>
        inline Iter insert(ConstIter _it, InputIter _first, InputIter _last)
        {
            Size idiff = _last - _first;
            Size index = (_it - begin());
            Size diff = m_count - index;
            Size mc = m_count + idiff;

            if (capacity() < mc)
            {
                reserve(max(mc, m_count * 2));
            }

            Size fidx = index + diff - 1;
            Size iidx = fidx + idiff;
            for (Size i = 0; i < diff; ++i)
            {
                new (reinterpret_cast<T *>(m_data.ptr) + iidx - i) T(std::move((*this)[fidx - i]));
            }

            for (Size i = 0; _first != _last; ++_first, ++i)
            {
                new (reinterpret_cast<T *>(m_data.ptr) + index + i) T(*_first);
            }

            m_count += idiff;
            return begin() + index;
        }

        inline Iter insert(ConstIter _it, const T & _val)
        {
            return insert(_it, &_val, &_val + 1);
        }

        inline Iter insert(ConstIter _it, T && _val)
        {
            Size index = (_it - begin());
            Size diff = m_count - index;
            Size mc = m_count + 1;

            if (capacity() < mc)
            {
                reserve(max(mc, m_count * 2));
            }

            Size fidx = index + diff - 1;
            Size iidx = fidx + 1;
            for (Size i = 0; i < diff; ++i)
            {
                new (reinterpret_cast<T *>(m_data.ptr) + iidx - i) T(std::move((*this)[fidx - i]));
            }

            new (reinterpret_cast<T *>(m_data.ptr) + index) T(std::forward<T>(_val));

            m_count++;
            return begin() + index;
        }

        inline Iter remove(ConstIter _first, ConstIter _last)
        {
            Size diff = end() - _last;
            Size idiff = _last - _first;
            Size index = (_first - begin());
            Size endIndex = m_count - diff;

            //fill the resulting gap if needed by shifting the remaining elements down
            if (diff)
            {
                for (Size i = 0; i < diff; ++i)
                {
                    (*this)[index + i] = std::move((*this)[endIndex + i]);
                }
            }

            //call the destructors of the removed elements that were not overwritten
            for (Size i = 0; i < idiff; ++i)
            {
                (*this)[index + diff + i].~T();
            }

            m_count -= idiff;
            return begin() + index;
        }

        inline void removeLast()
        {
            (reinterpret_cast<T *>(m_data.ptr)[m_count - 1]).~T();
            m_count--;
        }

        inline Iter remove(Iter _it)
        {
            return remove(_it, _it + 1);
        }

        inline void clear()
        {
            for (auto & el : *this)
            {
                el.~T();
            }
            m_count = 0;
        }

        inline void deallocate()
        {
            if (m_data)
            {
                //call the destructors
                clear();
                //and release the memory
                m_allocator->deallocate(m_data);
                m_data = {nullptr, 0};
            }
        }

        inline Allocator & allocator() const
        {
            STICK_ASSERT(m_allocator);
            return *m_allocator;
        }

        inline bool isEmpty() const
        {
            return m_count == 0;
        }

        inline const T & operator [](Size _index) const
        {
            return reinterpret_cast<T *>(m_data.ptr)[_index];
        }

        inline T & operator [](Size _index)
        {
            return reinterpret_cast<T *>(m_data.ptr)[_index];
        }

        inline Iter begin()
        {
            return (Iter)m_data.ptr;
        }

        inline ConstIter begin() const
        {
            return (ConstIter)m_data.ptr;
        }

        inline Iter end()
        {
            return (Iter)m_data.ptr + m_count;
        }

        inline ConstIter end() const
        {
            return (ConstIter)m_data.ptr + m_count;
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

        inline Size byteCount() const
        {
            return m_count * sizeof(T);
        }

        inline const T * ptr() const
        {
            return (const T *)m_data.ptr;
        }

        inline T * ptr()
        {
            return (T *)m_data.ptr;
        }

        inline Size capacity() const
        {
            return m_data.size / sizeof(T);
        }

        inline T & first()
        {
            return (*this)[0];
        }

        inline const T & first() const
        {
            return (*this)[0];
        }

        inline T & last()
        {
            return (*this)[m_count - 1];
        }

        inline const T & last() const
        {
            return (*this)[m_count - 1];
        }

        inline void swap(DynamicArray & _other)
        {
            std::swap(_other.m_allocator, m_allocator);
            std::swap(_other.m_count, m_count);
            std::swap(_other.m_data, m_data);
        }


    private:

        mem::Block m_data;
        Size m_count;
        Allocator * m_allocator;
    };
}

#endif //STICK_DYNAMICARRAY_HPP
