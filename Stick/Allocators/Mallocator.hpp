#ifndef STICK_ALLOCATORS_MALLOCATOR_HPP
#define STICK_ALLOCATORS_MALLOCATOR_HPP

#include <Stick/Allocators/Block.hpp>
#include <stdlib.h>

namespace stick
{
    namespace mem
    {
        class STICK_API Mallocator
        {
        public:
            
            static constexpr Size alignment = 4;

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                if (_alignment <= sizeof(void *))
                    return {malloc(_byteCount), _byteCount};
                else
                {
                    //@TODO: check if c11 aligned_alloc is available on non posix platforms?
                    void * ptr = nullptr;
                    posix_memalign(&ptr, (Size)sizeof(void *), _byteCount);
                    return {ptr, _byteCount};
                }
            }

            inline void deallocate(const Block & _blk)
            {
                free(_blk.ptr);
            }
        };
    }
}

#endif //STICK_ALLOCATORS_MALLOCATOR_HPP
