#ifndef STICK_ALLOCATORS_BLOCK_HPP
#define STICK_ALLOCATORS_BLOCK_HPP

#include <Stick/Platform.hpp>

namespace stick
{
    namespace mem
    {
        struct STICK_API Block
        {
            void * ptr;
            Size size;
        };
    }
}

#endif //STICK_ALLOCATORS_BLOCK_HPP
