#ifndef STICK_STRING_HPP
#define STICK_STRING_HPP

#include <Stick/StickAllocator.hpp>
#include <Stick/StickIterator.hpp>
#include <Stick/StickUtility.hpp>
#include <string.h>

namespace stick
{
    class String
    {
    public:

        typedef char * Iter;

        typedef const char * ConstIter;

        typedef ReverseIterator<Iter> ReverseIter;

        typedef ReverseIterator<ConstIter> ReverseConstIter;


        String() :
        m_cStr(nullptr),
        m_length(0),
        m_allocator(&defaultAllocator())
        {

        }

        String(const char * _c, Allocator & _alloc = defaultAllocator()) :
        m_allocator(&_alloc)
        {
            m_length = strlen(_c);
            m_cStr = (char*)m_allocator->allocate(m_length + 1).ptr;
            strcpy(m_cStr, _c);
        }

        String(const String & _other) :
        m_length(_other.m_length),
        m_allocator(_other.m_allocator)
        {
            if(_other.m_cStr)
            {
                m_cStr = (char*)m_allocator->allocate(m_length + 1).ptr;
                strcpy(m_cStr, _other.m_cStr);
            }
        }

        String(String && _other) :
        m_cStr(move(_other.m_cStr)),
        m_length(move(_other.m_length)),
        m_allocator(move(_other.m_allocator))
        {

        }

        ~String()
        {
            if(m_cStr)
            {
                STICK_ASSERT(m_allocator);
                m_allocator->deallocate({m_cStr, m_length});
            }
        }

        inline String & operator = (const String & _other)
        {
            m_length = _other.m_length;
            m_allocator = _other.m_allocator;
            if(_other.m_cStr)
            {
                m_cStr = (char*)m_allocator->allocate(_other.m_length + 1).ptr;
                strcpy(m_cStr, _other.m_cStr);
            }

            return *this;
        }

        inline String & operator = (String && _other)
        {
            m_cStr = move(_other.m_cStr);
            m_allocator = move(_other.m_allocator);
            m_length = move(_other.m_length);

            return *this;
        }

        inline String & operator = (const char * _other)
        {   
            if(!m_allocator)
                m_allocator = &defaultAllocator();

            m_length = strlen(_other);
            m_cStr = (char*)m_allocator->allocate(m_length + 1).ptr;
            strcpy(m_cStr, _other);

            return *this;
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
            return strcmp(m_cStr, _b.m_cStr) == 0;
        }

        inline bool operator != (const String & _b) const
        {
            return !(*this == _b);
        }

        inline bool operator == (const char * _str) const
        {
            return strcmp(m_cStr, _str) == 0;
        }

        inline bool operator != (const char * _str) const
        {
            return !(*this == _str);
        }

        inline Size length() const
        {
            return m_length;
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

    private:

        char * m_cStr;
        Size m_length;
        Allocator * m_allocator;
    };
}

#endif //STICK_STRING_HPP
