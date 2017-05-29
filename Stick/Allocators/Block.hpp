#ifndef STICK_ALLOCATORS_BLOCK_HPP
#define STICK_ALLOCATORS_BLOCK_HPP

#include <Stick/Platform.hpp>

namespace stick
{
    namespace mem
    {
        struct STICK_API Block
        {
            inline Block(void * _ptr = nullptr, Size _s = 0) :
                ptr(_ptr),
                size(_s)
            {
            }

            inline Block(const Block &) = default;
            inline Block(Block &&) = default;
            Block & operator = (const Block &) = default;
            Block & operator = (Block &&) = default;

            inline explicit operator bool() const
            {
                return ptr != nullptr;
            }

            inline UPtr end() const
            {
                return reinterpret_cast<UPtr>(ptr) + size;
            }

            void * ptr;
            Size size;
        };
    }
}

#endif //STICK_ALLOCATORS_BLOCK_HPP
