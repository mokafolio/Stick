#ifndef STICK_STICKFILEUTILITIES_HPP
#define STICK_STICKFILEUTILITIES_HPP

#include <Stick/StickString.hpp>
#include <Stick/StickDynamicArray.hpp>
#include <Stick/StickURI.hpp>

namespace stick
{
    typedef DynamicArray<char> ByteArray;

    ByteArray loadBinaryFile();

    String loadTexFile();
}

#endif //STICK_STICKFILEUTILITIES_HPP
