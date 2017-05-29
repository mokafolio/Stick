#ifndef STICK_ALLOCATORS_NOALLOCATOR_HPP
#define STICK_ALLOCATORS_NOALLOCATOR_HPP

#include <Stick/Allocators/Block.hpp>
#include <stdlib.h>

namespace stick
{
    namespace mem
    {
        class STICK_API NoAllocator
        {
        public:
            
            static constexpr Size alignment = 0;

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                return {nullptr, 0};
            }

            inline void deallocate(const Block & _blk)
            {

            }
        };
    }
}

#endif //STICK_ALLOCATORS_NOALLOCATOR_HPP
