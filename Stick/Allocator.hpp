#ifndef STICK_ALLOCATOR_HPP
#define STICK_ALLOCATOR_HPP

#include <Stick/Platform.hpp>
#include <Stick/Utility.hpp>
#include <stdlib.h>

namespace stick
{
    struct STICK_API Block
    {
        void * ptr;
        Size byteCount;
    };

    struct STICK_API Allocator
    {
        virtual Block allocate(Size _byteCount) = 0;

        virtual Block reallocate(const Block & _block, Size _byteCount) = 0;

        virtual void deallocate(const Block & _block) = 0;

        template<class T, class...Args>
        inline T * create(Args&&..._args)
        {
            auto mem = allocate(sizeof(T));
            return new (mem.ptr) T(forward<Args>(_args)...);
        }
    };

    struct STICK_API Mallocator : public Allocator
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

    inline STICK_API Allocator & defaultAllocator()
    {
        static Mallocator s_mallocator;
        return s_mallocator;
    }

    template<class T>
    inline STICK_API void destroy(T * _obj, Allocator & _alloc = defaultAllocator())
    {
        if (_obj)
        {
            _obj->~T();
            _alloc.deallocate({_obj, sizeof(T)});
        }
    }
}

#endif //STICK_ALLOCATOR_HPP
