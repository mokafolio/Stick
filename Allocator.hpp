#ifndef STICK_ALLOCATOR_HPP
#define STICK_ALLOCATOR_HPP

#include <Stick/Platform.hpp>
#include <stdlib.h>

namespace stick
{
    struct Block
    {
        void * ptr;
        Size byteCount;
    };

    struct Allocator
    {
        virtual Block allocate(Size _byteCount) = 0;

        virtual Block reallocate(const Block & _block, Size _byteCount) = 0;

        virtual void deallocate(const Block & _block) = 0;
    };

    struct Mallocator : public Allocator
    {
        inline Block allocate(Size _byteCount) override
        {
            return {malloc(_byteCount), _byteCount};
        }

        inline Block reallocate(const Block & _block, Size _byteCount) override
        {
            return {realloc(_block.ptr, _byteCount), _byteCount};
        }

        inline void deallocate(const Block & _block) override
        {
            free(_block.ptr);
        }
    };

    inline Allocator & defaultAllocator()
    {
        static Mallocator s_mallocator;
        return s_mallocator;
    }
}

#endif //STICK_ALLOCATOR_HPP
