#ifndef STICK_STRING_HPP
#define STICK_STRING_HPP

#include <Stick/StickAllocator.hpp>
#include <Stick/StickIterator.hpp>
#include <Stick/StickUtility.hpp>
#include <string.h>

namespace stick
{
    namespace detail
    {
        struct _StringCopier;
    }

    class String
    {
        friend class detail::_StringCopier;

    public:

        typedef char * Iter;
        typedef const char * ConstIter;
        typedef ReverseIterator<Iter> ReverseIter;
        typedef ReverseIterator<ConstIter> ReverseConstIter;


        inline explicit String(Allocator & _alloc = defaultAllocator()) :
            m_cStr(nullptr),
            m_length(0),
            m_capacity(0),
            m_allocator(&_alloc)
        {

        }

        template<class ... Strings>
        inline static String concatWithAllocator(Allocator & _alloc, Strings ..._args);

        template<class ... Strings>
        inline static String concat(Strings ..._args);

        inline String(Size _size, Allocator & _alloc = defaultAllocator()) :
            m_cStr(nullptr),
            m_capacity(0),
            m_allocator(&_alloc)
        {
            resize(_size);
        }

        inline String(const char * _c, Allocator & _alloc = defaultAllocator()) :
            m_cStr(nullptr),
            m_capacity(0),
            m_allocator(&_alloc)
        {
            m_length = strlen(_c);
            reserve(m_length);
            strcpy(m_cStr, _c);
        }

        inline String(const String & _other) :
            m_cStr(nullptr),
            m_length(_other.m_length),
            m_capacity(0),
            m_allocator(_other.m_allocator)
        {
            if (_other.m_cStr)
            {
                reserve(_other.m_length);
                strcpy(m_cStr, _other.m_cStr);
            }
        }

        inline String(String && _other) :
            m_cStr(move(_other.m_cStr)),
            m_length(move(_other.m_length)),
            m_capacity(move(_other.m_capacity)),
            m_allocator(move(_other.m_allocator))
        {
            _other.m_cStr = nullptr;
        }

        template<class InputIter>
        inline String(InputIter _begin, InputIter _end, Allocator & _alloc = defaultAllocator()) :
            m_length(0),
            m_capacity(0),
            m_allocator(&_alloc)
        {
            resize(_end - _begin);
            Size index = 0;
            for (; _begin != _end; ++_begin, ++index)
            {
                (*this)[index] = *_begin;
            }
        }

        inline ~String()
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

        template <class ... Strings>
        inline void append(Strings ... _args);

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
            for (Size i = m_length; i < _count; ++i)
            {
                (*this)[i] = _c;
            }
            m_length = _count;
        }

        inline void reserve(Size _count)
        {
            if (_count <= m_capacity)
                return;

            char * old = m_cStr;
            m_cStr = static_cast<char *>(m_allocator->allocate(_count + 1).ptr);
            STICK_ASSERT(m_cStr != nullptr);
            if (old)
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

        inline void preAppend(Size _newLen)
        {
            if (m_capacity < _newLen)
                reserve(_newLen * 2);
            m_length = _newLen;
        }

        char * m_cStr;
        Size m_length;
        Size m_capacity;
        Allocator * m_allocator;
    };

    namespace detail
    {
        struct _StringCopier
        {
            inline static Size strLen(const String & _str)
            {
                return _str.length();
            }

            inline static Size strLen(const char * _str)
            {
                return strlen(_str);
            }

            inline static Size strLen(char _c)
            {
                return 1;
            }

            inline static int performCopy(String & _dest, Size & _off, const String & _src)
            {
                strcpy(_dest.m_cStr + _off, _src.m_cStr);
                _off += _src.length();
                return 0;
            }

            inline static int performCopy(String & _dest, Size & _off, const char * _src)
            {
                strcpy(_dest.m_cStr + _off, _src);
                _off += strlen(_src);
                return 0;
            }

            inline static int performCopy(String & _dest, Size & _off, char _src)
            {
                _dest[_off] = _src;
                ++_off;
                return 0;
            }
        };
    }

    template <class ... Strings>
    inline void String::append(Strings ... _args)
    {
        Size len = 0;
        int unpack[] {0, (len += detail::_StringCopier::strLen(_args), 0)...};
        Size off = m_length;
        preAppend(m_length + len);
        int unpack2[] {0, (detail::_StringCopier::performCopy(*this, off, _args))...};
    }

    template<class ... Strings>
    inline String String::concatWithAllocator(Allocator & _alloc, Strings ..._args)
    {
        Size len = 0;
        int unpack[] {0, (len += detail::_StringCopier::strLen(_args), 0)...};
        String ret(len, _alloc);
        Size off = 0;
        int unpack2[] {0, (detail::_StringCopier::performCopy(ret, off, _args))...};
        return ret;
    }

    template<class ... Strings>
    inline String String::concat(Strings ..._args)
    {
        Size len = 0;
        int unpack[] {0, (len += detail::_StringCopier::strLen(_args), 0)...};
        String ret(len);
        Size off = 0;
        int unpack2[] {0, (detail::_StringCopier::performCopy(ret, off, _args))...};
        return ret;
    }
}

#endif //STICK_STRING_HPP
