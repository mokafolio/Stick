#ifndef STICK_STICKSTRINGCONVERSION_HPP
#define STICK_STICKSTRINGCONVERSION_HPP

#include <stdio.h>
#include <Stick/StickString.hpp>

namespace stick
{   
    template<class T>
    inline String toString(T _i, Allocator & _alloc = defaultAllocator())
    {
        return String::toString(_i, _alloc);
    }

    template<class T>
    inline String toHexString(T _i, UInt32 _width = 0, bool _bUpperCase = true, bool _bShowBase = false, Allocator & _alloc = defaultAllocator())
    {
        return String::toHexString(_i, _width, _bUpperCase, _bShowBase, _alloc);
    }
}

#endif //STICK_STICKSTRINGCONVERSION_HPP
