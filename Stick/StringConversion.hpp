#ifndef STICK_STICKSTRINGCONVERSION_HPP
#define STICK_STICKSTRINGCONVERSION_HPP

#include <stdio.h>
#include <Stick/String.hpp>

namespace stick
{
    /**
    * @brief Converts a numeric String to an Int16.
    */
    inline Int16 toInt16(const String & _str)
    {
        return atoi(_str.cString());
    }

    /**
     * @brief Converts a numeric String to an UInt16.
     */
    inline UInt16 toUInt16(const String & _str)
    {
        return atoi(_str.cString());
    }

    /**
     * @brief Converts a numeric String to an Int32.
     */
    inline Int32 toInt32(const String & _str)
    {
        return atoi(_str.cString());
    }

    /**
     * @brief Converts a numeric String to an UInt32.
     */
    inline UInt32 toUInt32 (const String & _str)
    {
        return atoi(_str.cString());
    }

    /**
     * @brief Converts a numeric String to an Int64.
     */
    inline Int64 toInt64(const String & _str)
    {
        return atoll(_str.cString());
    }

    /**
     * @brief Converts a numeric String to an UInt64.
     */
    inline UInt64 toUInt64(const String & _str)
    {
        return atoll(_str.cString());
    }

    inline Float32 toFloat32(const String & _str)
    {
        return atof(_str.cString());
    }

    inline Float64 toFloat64(const String & _str)
    {
        return atof(_str.cString());
    }

    template<class T>
    inline String toString(T _i, Allocator & _alloc = defaultAllocator())
    {
        return String::toString(std::forward<T>(_i), _alloc);
    }

    template<class T>
    inline String toHexString(T _i, UInt32 _width = 0, bool _bUpperCase = true, bool _bShowBase = false, Allocator & _alloc = defaultAllocator())
    {
        return String::toHexString(_i, _width, _bUpperCase, _bShowBase, _alloc);
    }

    namespace detail
    {
        template<class T>
        inline T convert(const String & _str);

        template<>
        inline const String & convert<const String &>(const String & _str)
        {
            return _str;
        }

        template<>
        inline bool convert<bool>(const String & _str)
        {
            return _str == "true" || _str == "1";
        }

        template<>
        inline Float32 convert<Float32>(const String & _str)
        {
            return toFloat32(_str);
        }

        template<>
        inline Float64 convert<Float64>(const String & _str)
        {
            return toFloat64(_str);
        }

        template<>
        inline Int32 convert<Int32>(const String & _str)
        {
            return toInt32(_str);
        }

        template<>
        inline UInt32 convert<UInt32>(const String & _str)
        {
            return toUInt32(_str);
        }

        template<>
        inline Int64 convert<Int64>(const String & _str)
        {
            return toInt64(_str);
        }

        template<>
        inline UInt64 convert<UInt64>(const String & _str)
        {
            return toUInt64(_str);
        }
    }

    template<class T>
    inline T convertString(const String & _str)
    {
        return detail::convert<T>(_str);
    }
}

#endif //STICK_STICKSTRINGCONVERSION_HPP
