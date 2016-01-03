#ifndef STICK_STRING_HPP
#define STICK_STRING_HPP

#include <Stick/StickAllocator.hpp>
#include <Stick/StickIterator.hpp>
#include <Stick/StickUtility.hpp>
#include <string.h>

#include <iostream>

namespace stick
{
    class String
    {
    public:

        typedef char * Iter;
        typedef const char * ConstIter;
        typedef ReverseIterator<Iter> ReverseIter;
        typedef ReverseIterator<ConstIter> ReverseConstIter;


        explicit String(Allocator & _alloc = defaultAllocator()) :
            m_cStr(nullptr),
            m_length(0),
            m_capacity(0),
            m_allocator(&_alloc)
        {

        }

        String(const char * _c, Allocator & _alloc = defaultAllocator()) :
            m_allocator(&_alloc),
            m_cStr(0),
            m_capacity(0)
        {
            std::cout<<"CA"<<std::endl;
            m_length = strlen(_c);
            reserve(m_length);
            strcpy(m_cStr, _c);
        }

        String(const String & _other) :
            m_length(_other.m_length),
            m_capacity(0),
            m_allocator(_other.m_allocator)
        {
            if (_other.m_cStr)
            {
                std::cout<<"A"<<std::endl;
                reserve(_other.m_length);
                strcpy(m_cStr, _other.m_cStr);
            }
        }

        String(String && _other) :
            m_cStr(move(_other.m_cStr)),
            m_length(move(_other.m_length)),
            m_capacity(move(_other.m_capacity)),
            m_allocator(move(_other.m_allocator))
        {
            _other.m_cStr = nullptr;
        }

        template<class InputIter>
        String(InputIter _begin, InputIter _end, Allocator & _alloc = defaultAllocator()) :
        m_length(0),
        m_capacity(0),
        m_allocator(&_alloc)
        {
            m_length = _end - _begin;
            reserve(m_length);
            Size index = 0;
            for(; _begin != _end; ++_begin, ++index)
            {
                (*this)[index] = *_begin;
            }
        }

        ~String()
        {
            if (m_cStr)
            {
                m_allocator->deallocate({m_cStr, m_capacity + 1});
            }
        }

        inline String & operator = (const String & _other)
        {
            m_length = _other.m_length;
            m_allocator = _other.m_allocator;
            if (_other.m_cStr)
            {
                reserve(m_length);
                strcpy(m_cStr, _other.m_cStr);
            }

            return *this;
        }

        inline String & operator = (String && _other)
        {
            m_cStr = move(_other.m_cStr);
            m_allocator = move(_other.m_allocator);
            m_length = move(_other.m_length);
            _other.m_cStr = nullptr;
            return *this;
        }

        inline String & operator = (const char * _other)
        {
            if (!m_allocator)
                m_allocator = &defaultAllocator();

            m_length = strlen(_other);
            reserve(m_length);
            strcpy(m_cStr, _other);

            return *this;
        }

        inline void append(const String & _a)
        {
            Size ol = m_length;
            m_length = m_length + _a.m_length;
            reserve(m_length * 2);
            strcpy(m_cStr + ol, _a.m_cStr);
        }

        inline void append(const char * _cstr)
        {
            Size ol = m_length;
            m_length = m_length + strlen(_cstr);
            reserve(m_length * 2);
            strcpy(m_cStr + ol, _cstr);
        }

        inline char operator [](Size _index) const
        {
            return *(begin() + _index);
        }

        inline char & operator [](Size _index)
        {
            return *(begin() + _index);
        }

        inline bool operator == (const String & _b) const
        {
            if (!m_cStr && !_b.m_cStr) return true;
            else if (!m_cStr) return false;
            return strcmp(m_cStr, _b.m_cStr) == 0;
        }

        inline bool operator != (const String & _b) const
        {
            return !(*this == _b);
        }

        inline bool operator == (const char * _str) const
        {
            if (!m_cStr && !_str) return true;
            else if (!m_cStr) return false;
            return strcmp(m_cStr, _str) == 0;
        }

        inline bool operator != (const char * _str) const
        {
            return !(*this == _str);
        }

        inline bool operator < (const String & _str) const
        {
            if (!m_cStr) return true;
            return strcmp(m_cStr, _str.m_cStr) < 0;
        }

        inline bool operator > (const String & _str) const
        {
            if (!m_cStr) return false;
            return strcmp(m_cStr, _str.m_cStr) > 0;
        }

        inline bool operator < (const char * _str) const
        {
            if (!m_cStr) return true;
            return strcmp(m_cStr, _str) < 0;
        }

        inline bool operator > (const char * _str) const
        {
            if (!m_cStr) return false;
            return strcmp(m_cStr, _str) > 0;
        }

        inline void resize(Size _count)
        {
            reserve(_count);
            m_length = _count;
        }

        inline void resize(Size _count, char _c)
        {
            reserve(_count);
            for(Size i=m_length; i < _count; ++i)
            {
                (*this)[i] = _c;
            }
            m_length = _count;
        }

        inline void reserve(Size _count)
        {
            if(_count <= m_capacity)
                return;

            char * old = m_cStr;
            m_cStr = static_cast<char*>(m_allocator->allocate(_count + 1).ptr);
            if(old)
            {
                strcpy(m_cStr, old);
                m_allocator->deallocate({old, m_capacity + 1});
            }
            m_capacity = _count;
        }

        inline Size length() const
        {
            return m_length;
        }

        inline Size capacity() const
        {
            return m_capacity;
        }

        inline Iter begin()
        {
            return m_cStr;
        }

        inline ConstIter begin() const
        {
            return m_cStr;
        }

        inline Iter end()
        {
            return m_cStr + m_length;
        }

        inline ConstIter end() const
        {
            return m_cStr + m_length;
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

        inline const char * cString() const
        {
            return m_cStr;
        }

        inline bool isEmpty() const
        {
            return m_length == 0;
        }

    private:

        char * m_cStr;
        Size m_length;
        Size m_capacity;
        Allocator * m_allocator;
    };
}

#endif //STICK_STRING_HPP
