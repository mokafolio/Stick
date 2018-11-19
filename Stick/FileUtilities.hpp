#ifndef STICK_FILEUTILITIES_HPP
#define STICK_FILEUTILITIES_HPP

#include <Stick/DynamicArray.hpp>
#include <Stick/Result.hpp>
#include <Stick/String.hpp>
#include <Stick/URI.hpp>

namespace stick
{
    
typedef DynamicArray<char> ByteArray;

Result<ByteArray> loadBinaryFile(const String & _uri, Allocator & _alloc = defaultAllocator());

Result<String> loadTextFile(const String & _uri, Allocator & _alloc = defaultAllocator());

Error saveBinaryFile(const ByteArray & _data, const String & _uri);

Error saveTextFile(const String & _text, const String & _uri);

} // namespace stick

#endif // STICK_FILEUTILITIES_HPP
