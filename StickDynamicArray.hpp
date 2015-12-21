#ifndef STICK_DYNAMICARRAY_HPP
#define STICK_DYNAMICARRAY_HPP

#include <Stick/StickAllocator.hpp>
#include <Stick/StickIterator.hpp>
#include <Stick/StickUtility.hpp>

#include <iostream>

namespace stick
{
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
            m_data( {nullptr, 0}),
                m_elementCount(0),
                m_allocator(&_alloc)
        {

        }

        DynamicArray(Size _size, Allocator & _alloc = defaultAllocator()) :
            m_elementCount(_size),
            m_allocator(&_alloc)
        {
            m_data = (T *)m_allocator->allocate(m_elementCount * sizeof(T));
        }

        DynamicArray(const DynamicArray & _other) :
            m_elementCount(_other.m_elementCount),
            m_allocator(_other.m_allocator)
        {
            if (m_elementCount)
            {
                resize(m_elementCount);
                for (Size i = 0; i < m_elementCount; ++i)
                {
                    *this[i] = _other[i];
                }
            }
        }

        DynamicArray(DynamicArray && _other) :
            m_data(move(_other.m_data)),
            m_elementCount(move(_other.m_elementCount)),
            m_allocator(move(_other.m_allocator))
        {
            //we don't want other to deallocate anything
            _other.m_elementCount = 0;
        }

        ~DynamicArray()
        {
            if (m_elementCount)
            {
                //call the destructors
                clear();
                //and release the memory
                m_allocator->deallocate(m_data);
            }
        }

        inline DynamicArray & operator = (const DynamicArray & _other)
        {
            m_elementCount = _other.m_elementCount;
            m_allocator = _other.m_allocator;
            resize(m_elementCount);
            for (Size i = 0; i < m_elementCount; ++i)
            {
                *this[i] = _other[i];
            }
            return *this;
        }

        inline DynamicArray & operator = (DynamicArray && _other)
        {
            m_data = move(_other.m_data);
            m_allocator = move(_other.m_allocator);
            m_elementCount = move(_other.m_elementCount);
            _other.m_elementCount = 0;

            return *this;
        }

        inline void resize(Size _s)
        {
            reserve(_s);
            m_elementCount = _s;
        }

        inline void reserve(Size _s)
        {
            if (_s > capacity())
            {
                auto blk = m_allocator->allocate(_s * sizeof(T));
                if (blk.ptr != m_data.ptr)
                {
                    for (Size i = 0; i < m_elementCount; ++i)
                    {
                        reinterpret_cast<T *>(blk.ptr)[i] = reinterpret_cast<T *>(m_data.ptr)[i];
                    }
                    m_allocator->deallocate(m_data);
                }
                m_data = blk;
                //TODO: do this for POD types?
                //m_data = m_allocator->reallocate(m_data, _s * sizeof(T));
            }
        }

        inline void append(const T & _element)
        {
            if (capacity() <= m_elementCount)
            {
                reserve(max((Size)1, m_elementCount * 2));
            }
            (*this)[m_elementCount++] = _element;
        }

        template<class InputIter>
        inline Iter insert(ConstIter _it, InputIter _first, InputIter _last)
        {
            Size idiff = _last - _first;
            Size index = (_it - begin());
            Size diff = m_elementCount - index;

            if (capacity() < m_elementCount + idiff)
            {
                reserve(max(idiff, m_elementCount * 2));
            }

            Size fidx = index + diff - 1;
            Size iidx = fidx + idiff;
            for (Size i = 0; i < diff; ++i)
            {
                (*this)[iidx - i] = (*this)[fidx - i];
            }

            for (Size i = 0; _first != _last; ++_first, ++i)
            {
                (*this)[index + i] = *_first;
            }

            m_elementCount += idiff;
            return begin() + index;
        }

        inline Iter insert(ConstIter _it, const T & _val)
        {
            return insert(_it, &_val, &_val + 1);
        }

        inline Iter remove(ConstIter _first, ConstIter _last)
        {
            Size diff = end() - _last;
            Size idiff = _last - _first;
            Size index = (_first - begin());
            Size endIndex = m_elementCount - diff;

            //call the destructors of the removed elements
            for (Size i = 0; i < idiff; ++i)
            {
                (*this)[index + i].~T();
            }

            //fill the resulting gap if needed by shifting the remaining elements down
            if (diff)
            {
                for (Size i = 0; i < diff; ++i)
                {
                    (*this)[index + i] = (*this)[endIndex + i];
                }
            }

            m_elementCount -= idiff;
            return begin() + index;
        }

        inline void removeBack()
        {
            (reinterpret_cast<T *>(m_data.ptr)[m_elementCount - 1]).~T();
            m_elementCount--;
        }

        inline void clear()
        {
            for (auto & el : *this)
            {
                el.~T();
            }
            m_elementCount = 0;
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
            return (Iter)m_data.ptr + m_elementCount;
        }

        inline ConstIter end() const
        {
            return (ConstIter)m_data.ptr + m_elementCount;
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

        inline Size elementCount() const
        {
            return m_elementCount;
        }

        inline Size byteCount() const
        {
            return m_elementCount * sizeof(T);
        }

        inline const T * ptr() const
        {
            return (const T *)m_data.ptr;
        }

        inline Size capacity() const
        {
            return m_data.byteCount / sizeof(T);
        }

        inline T & front()
        {
            return (*this)[0];
        }

        inline const T & front() const
        {
            return (*this)[0];
        }

        inline T & back()
        {
            return (*this)[m_elementCount - 1];
        }

        inline const T & back() const
        {
            return (*this)[m_elementCount - 1];
        }

    private:

        Block m_data;
        Size m_elementCount;
        Allocator * m_allocator;
    };
}

#endif //STICK_DYNAMICARRAY_HPP
