#ifndef STICK_STRING_HPP
#define STICK_STRING_HPP

#include <Stick/Allocator.hpp>
#include <Stick/Iterator.hpp>
#include <Stick/Utility.hpp>

// for string hashing
#include <Stick/Hash.hpp>
#include <Stick/Private/MurmurHash2.hpp>

#include <string.h>
#include <cctype> //for toupper
#include <algorithm> //for transform

namespace stick
{
    namespace detail
    {
        struct _StringCopier;

        template<class...Args>
        inline int variadicStringLength(const char * format, Args..._args)
        {
            int result = snprintf(NULL, 0, format, _args...);
            return result; // ignore extra byte for \0
        }
    }

    struct AppendVariadicFlag {};

    class String
    {
        friend class detail::_StringCopier;

    public:

        typedef char * Iter;
        typedef const char * ConstIter;
        typedef ReverseIterator<Iter> ReverseIter;
        typedef ReverseIterator<ConstIter> ReverseConstIter;
        static constexpr Size InvalidIndex = -1;


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
            m_length(0),
            m_capacity(0),
            m_allocator(&_alloc)
        {
            reserve(_size);
        }

        inline String(const char * _c, Allocator & _alloc = defaultAllocator()) :
            m_cStr(nullptr),
            m_length(0),
            m_capacity(0),
            m_allocator(&_alloc)
        {
            Size l = strlen(_c);
            if (l)
            {
                resize(l);
                strcpy(m_cStr, _c);
            }
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
            m_cStr(std::move(_other.m_cStr)),
            m_length(std::move(_other.m_length)),
            m_capacity(std::move(_other.m_capacity)),
            m_allocator(std::move(_other.m_allocator))
        {
            _other.m_cStr = nullptr;
        }

        template<class InputIter>
        inline String(InputIter _begin, InputIter _end, Allocator & _alloc = defaultAllocator()) :
            m_cStr(nullptr),
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
            deallocate();
        }

        inline String & operator = (const String & _other)
        {
            deallocate();
            m_allocator = _other.m_allocator;
            if (_other.m_cStr)
            {
                resize(_other.m_length);
                strcpy(m_cStr, _other.m_cStr);
            }

            return *this;
        }

        inline String & operator = (String && _other)
        {
            deallocate();
            m_cStr = std::move(_other.m_cStr);
            m_allocator = std::move(_other.m_allocator);
            m_length = std::move(_other.m_length);
            m_capacity = std::move(_other.m_capacity);
            _other.m_cStr = nullptr;
            return *this;
        }

        inline String & operator = (const char * _other)
        {
            if (m_cStr)
                deallocate();

            if (!m_allocator)
                m_allocator = &defaultAllocator();

            Size len = strlen(_other);
            m_length = 0;
            if (len)
            {
                resize(strlen(_other));
                strcpy(m_cStr, _other);
            }

            return *this;
        }

        template <class ... Strings>
        inline void append(AppendVariadicFlag _flag, Strings ... _args);

        template<class...Args>
        inline void appendFormatted(const char * _fmt, Args..._args)
        {
            int len = detail::variadicStringLength(_fmt, _args...);
            Size off = m_length;
            preAppend(m_length + len);
            int result = snprintf(m_cStr + off, len + 1, _fmt, _args...);
            STICK_ASSERT(len == result);
        }

        inline void append(const String & _str)
        {
            if (!_str.m_length) return;
            Size off = m_length;
            preAppend(m_length + _str.length());
            strcpy(m_cStr + off, _str.m_cStr);
        }

        inline void append(const char * _cStr)
        {
            Size off = m_length;
            preAppend(m_length + strlen(_cStr));
            strcpy(m_cStr + off, _cStr);
        }

        inline void append(const char * _cStr, Size _count)
        {
            Size off = m_length;
            preAppend(m_length + _count);
            memcpy(m_cStr + off, _cStr, _count);
        }

        inline void append(char _c)
        {
            Size off = m_length;
            preAppend(m_length + 1);
            (*this)[off] = _c;
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
            //TODO: Make this nicer!!!1111
            if (isEmpty() && _b.isEmpty())
            {
                return true;
            }
            else if (isEmpty() || _b.isEmpty())
                return false;
            return strcmp(m_cStr, _b.m_cStr) == 0;
        }

        inline bool operator != (const String & _b) const
        {
            return !(*this == _b);
        }

        inline bool operator == (const char * _str) const
        {
            if (isEmpty() && (!_str || (_str && strlen(_str) == 0))) return true;
            else if (isEmpty() || !_str) return false;
            return strcmp(m_cStr, _str) == 0;
        }

        inline bool operator != (const char * _str) const
        {
            return !(*this == _str);
        }

        inline bool operator < (const String & _str) const
        {
            if (!m_cStr) return true;
            if (!_str.m_cStr) return false;
            return strcmp(m_cStr, _str.m_cStr) < 0;
        }

        inline bool operator > (const String & _str) const
        {
            if (!m_cStr) return false;
            if (!_str.m_cStr) return true;
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

        inline bool operator <= (const String & _str) const
        {
            if (!m_cStr && !_str.m_cStr) return true;
            if (!_str.m_cStr || !m_cStr) return false;
            return strcmp(m_cStr, _str.m_cStr) < 0;
        }

        inline bool operator >= (const String & _str) const
        {
            if (!m_cStr && !_str.m_cStr) return true;
            if (!_str.m_cStr || m_cStr) return false;
            return strcmp(m_cStr, _str.m_cStr) >= 0;
        }

        inline bool operator <= (const char * _str) const
        {
            if (!m_cStr) return true;
            return strcmp(m_cStr, _str) <= 0;
        }

        inline bool operator >= (const char * _str) const
        {
            if (!m_cStr) return false;
            return strcmp(m_cStr, _str) >= 0;
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
            Size s = _count + 1;
            m_cStr = static_cast<char *>(m_allocator->allocate(s, alignof(char)).ptr);
            //needed as allocator cannot guarantee that the memory is zeroed out
            //TODO: Add a allocateZeroed function to allocator? Would allow us to use calloc
            memset(m_cStr, 0, s);
            STICK_ASSERT(m_cStr != nullptr);
            if (old)
            {
                strcpy(m_cStr, old);
                m_allocator->deallocate({old, m_capacity + 1});
            }
            m_capacity = _count;
        }

        inline Size findIndex(char _c, Size _startIndex = 0) const
        {
            //STICK_ASSERT(_startIndex < m_length);
            for (; _startIndex < m_length; ++_startIndex)
            {
                if ((*this)[_startIndex] == _c)
                    return _startIndex;
            }
            return InvalidIndex;
        }

        inline Size rfindIndex(char _c, Size _startIndex = InvalidIndex) const
        {
            _startIndex = _startIndex == InvalidIndex ? m_length - 1 : _startIndex;
            //STICK_ASSERT(_startIndex < m_length);
            for (; _startIndex > 0; --_startIndex)
            {
                if ((*this)[_startIndex] == _c)
                    return _startIndex;
            }
            return InvalidIndex;
        }

        inline Size findIndex(const String & _str, Size _startIndex = 0) const
        {
            //STICK_ASSERT(_startIndex < m_length);
            for (; _startIndex < m_length - _str.m_length; ++_startIndex)
            {
                bool bBreak = false;
                for (Size i = 0; i < _str.m_length; ++i)
                {
                    if (_str[i] != (*this)[_startIndex + i])
                    {
                        bBreak = true;
                        break;
                    }
                }
                if (!bBreak)
                    return _startIndex;
            }
            return InvalidIndex;
        }

        inline Size rfindIndex(const String & _str, Size _startIndex = InvalidIndex) const
        {
            _startIndex = _startIndex == InvalidIndex ? m_length - _str.m_length : _startIndex;
            //STICK_ASSERT(_startIndex < m_length);
            for (; _startIndex > 0; --_startIndex)
            {
                bool bBreak = false;
                for (Size i = 0; i < _str.m_length; ++i)
                {
                    if (_str[i] != (*this)[_startIndex + i])
                    {
                        bBreak = true;
                        break;
                    }
                }
                if (!bBreak)
                    return _startIndex;
            }
            return InvalidIndex;
        }

        inline String sub(Size _pos, Size _length = InvalidIndex) const
        {
            return sub(_pos, _length, allocator());
        }

        inline String sub(Size _pos, Size _length, Allocator & _alloc) const
        {
            return String(begin() + _pos, _length == InvalidIndex ? begin() + m_length : begin() + _pos + _length, _alloc);
        }

        inline Size length() const
        {
            return m_length;
        }

        inline Size capacity() const
        {
            return m_capacity;
        }

        inline void clear()
        {
            if (m_length)
            {
                memset(m_cStr, 0, m_capacity);
                m_length = 0;
            }
        }

        inline void deallocate()
        {
            if (m_cStr)
            {
                m_allocator->deallocate({m_cStr, m_capacity + 1});
                m_capacity = 0;
                m_length = 0;
                m_cStr = nullptr;
            }
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

        inline void * ptr()
        {
            return m_cStr;
        }

        inline const void * ptr() const
        {
            return m_cStr;
        }

        inline bool isEmpty() const
        {
            return m_length == 0;
        }

        inline Allocator & allocator() const
        {
            STICK_ASSERT(m_allocator);
            return *m_allocator;
        }

        inline String toUpper() const
        {
            String ret(*this);
            std::transform(ret.begin(), ret.end(), ret.begin(), [](UInt8 _c) { return std::toupper(_c); });
            return ret;
        }

        inline String toLower() const
        {
            String ret(*this);
            std::transform(ret.begin(), ret.end(), ret.begin(), [](UInt8 _c) { return std::tolower(_c); });
            return ret;
        }

        inline static String toString(Int32 _i, Allocator & _alloc = defaultAllocator())
        {
            String ret(_alloc);
            Size len = snprintf(NULL, 0, "%i", _i);
            ret.resize(len);
            snprintf(ret.m_cStr, len + 1, "%i", _i);
            return ret;
        }

        inline static String toString(Int64 _i, Allocator & _alloc = defaultAllocator())
        {
            String ret(_alloc);
            Size len = snprintf(NULL, 0, "%lli", _i);
            ret.resize(len);
            snprintf(ret.m_cStr, len + 1, "%lli", _i);
            return ret;
        }

        inline static String toString(UInt32 _i, Allocator & _alloc = defaultAllocator())
        {
            String ret(_alloc);
            Size len = snprintf(NULL, 0, "%u", _i);
            ret.resize(len);
            snprintf(ret.m_cStr, len + 1, "%u", _i);
            return ret;
        }

        inline static String toString(UInt64 _i, Allocator & _alloc = defaultAllocator())
        {
            String ret(_alloc);
            Size len = snprintf(NULL, 0, "%llu", _i);
            ret.resize(len);
            snprintf(ret.m_cStr, len + 1, "%llu", _i);
            return ret;
        }

        inline static String toString(Float64 _i, Allocator & _alloc = defaultAllocator())
        {
            String ret(_alloc);
            Size len = snprintf(NULL, 0, "%f", _i);
            ret.resize(len);
            snprintf(ret.m_cStr, len + 1, "%f", _i);
            return ret;
        }

        inline static String toString(Float32 _i, Allocator & _alloc = defaultAllocator())
        {
            return toString(static_cast<Float64>(_i), _alloc);
        }

        inline static String toHexString(UInt64 _i, UInt32 _width = 0, bool _bUpperCase = true, bool _bShowBase = false, Allocator & _alloc = defaultAllocator())
        {
            String ret(_alloc);
            //TODO: optimize fmt string generation
            String fmtString(_alloc);
            fmtString.reserve(64);
            if (_bShowBase)
                fmtString.append("0x");
            fmtString.append(AppendVariadicFlag(), "%0", toString(_width, _alloc));
            if (_bUpperCase)
                fmtString.append("X");
            else
                fmtString.append("x");

            Size len = snprintf(NULL, 0, fmtString.cString(), _i);
            ret.resize(len);
            snprintf(ret.m_cStr, len + 1, fmtString.cString(), _i);
            return ret;
        }

        inline static String toHexString(UInt32 _i, UInt32 _width = 0, bool _bUpperCase = true, bool _bShowBase = false, Allocator & _alloc = defaultAllocator())
        {
            return String::toHexString(static_cast<UInt64>(_i), _width, _bUpperCase, _bShowBase, _alloc);
        }

        inline static String toHexString(Int64 _i, UInt32 _width = 0, bool _bUpperCase = true, bool _bShowBase = false, Allocator & _alloc = defaultAllocator())
        {
            if (_i >= 0)
            {
                return String::toHexString(static_cast<UInt64>(_i), _width, _bUpperCase, _bShowBase, _alloc);
            }
            else
            {
                //TODO: this could be easily made more efficient as this will still be two memory allocations
                return String::concat("-", toHexString(static_cast<UInt64>(labs(_i)), _width, _bUpperCase, _bShowBase, _alloc));
            }
        }

        inline static String toHexString(Int32 _i, UInt32 _width = 0, bool _bUpperCase = true, bool _bShowBase = false, Allocator & _alloc = defaultAllocator())
        {
            return String::toHexString(static_cast<Int64>(_i), _width, _bUpperCase, _bShowBase, _alloc);
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
                if (_src.m_cStr)
                {
                    strcpy(_dest.m_cStr + _off, _src.m_cStr);
                    _off += _src.length();
                }
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
    inline void String::append(AppendVariadicFlag _flag, Strings ... _args)
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
        String ret(_alloc);
        ret.resize(len);
        Size off = 0;
        int unpack2[] {0, (detail::_StringCopier::performCopy(ret, off, _args))...};
        return ret;
    }

    template<class ... Strings>
    inline String String::concat(Strings ..._args)
    {
        Size len = 0;
        int unpack[] {0, (len += detail::_StringCopier::strLen(_args), 0)...};
        String ret;
        ret.resize(len);
        Size off = 0;
        int unpack2[] {0, (detail::_StringCopier::performCopy(ret, off, _args))...};
        return ret;
    }

    template<>
    struct DefaultHash<String>
    {
        Size operator()(const String & _str) const
        {
            return detail::murmur2(_str.cString(), _str.length(), 0);
        }
    };
}

#endif //STICK_STRING_HPP
