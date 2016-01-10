#ifndef STICK_STICKFILEUTILITIES_HPP
#define STICK_STICKFILEUTILITIES_HPP

#include <Stick/String.hpp>
#include <Stick/DynamicArray.hpp>
#include <Stick/URI.hpp>
#include <Stick/Result.hpp>

namespace stick
{
    typedef DynamicArray<char> ByteArray;

    Result<ByteArray> loadBinaryFile(const URI & _uri);

    Result<String> loadTextFile(const URI & _uri);

    Error saveBinaryFile(const ByteArray & _data, const URI & _uri);

    Error saveTextFile(const String & _text, const URI & _uri);
}

#endif //STICK_STICKFILEUTILITIES_HPP
