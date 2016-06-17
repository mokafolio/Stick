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
}

#endif //STICK_STICKSTRINGCONVERSION_HPP
