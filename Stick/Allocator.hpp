#ifndef STICK_ALLOCATOR_HPP
#define STICK_ALLOCATOR_HPP

#include <Stick/Platform.hpp>
#include <Stick/Utility.hpp>
#include <algorithm>
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

        virtual Block allocateAligned(Size _byteCount, Size _alignment) = 0;

        virtual Block reallocate(const Block & _block, Size _byteCount) = 0;

        virtual void deallocate(const Block & _block) = 0;

        template<class T, class...Args>
        inline T * create(Args && ..._args)
        {
            static constexpr auto headerSize = sizeof(Size) + sizeof(Allocator *);
            static constexpr auto headerAdjustment = headerSize % alignof(T) == 0 ? 0 : headerSize + alignof(T) - headerSize % alignof(T);
            static constexpr auto totalSize = sizeof(T) + headerSize + headerAdjustment;
            printf("HEADER SIZE %lu ADJUST %lu %lu\n", headerSize, headerAdjustment, alignof(T));
            auto mem = allocateAligned(totalSize, alignof(T));
            char * bytePtr = reinterpret_cast<char *>(mem.ptr) + headerAdjustment;
            Allocator ** alloc = reinterpret_cast<Allocator **>(bytePtr);
            *alloc = this;
            Size * ptr = reinterpret_cast<Size *>(bytePtr + sizeof(Allocator *));
            *ptr = totalSize;
            return new (bytePtr + headerSize) T(std::forward<Args>(_args)...);
        }
    };

    struct STICK_API Mallocator : public Allocator
    {
        inline Block allocate(Size _byteCount) override
        {
            return {malloc(_byteCount), _byteCount};
        }

        inline Block allocateAligned(Size _byteCount, Size _alignment) override
        {
            //@TODO: check if c11 aligned_alloc is available on non posix platforms?
            void * ptr = nullptr;
            auto res = posix_memalign(&ptr, std::max(_alignment, (Size)sizeof(void*)), _byteCount);
            if(res == 0)
                return {ptr, _byteCount};
            else
                return {nullptr, 0};
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
    inline STICK_API void destroy(T * _obj)
    {
        if (_obj)
        {
            static constexpr auto headerSize = sizeof(Size) + sizeof(Allocator *);
            static constexpr auto headerAdjustment = headerSize % alignof(T) == 0 ? 0 : headerSize + alignof(T) - headerSize % alignof(T);
            _obj->~T();
            Allocator ** alloc = reinterpret_cast<Allocator **>(reinterpret_cast<char *>(_obj) - headerSize);
            (*alloc)->deallocate({reinterpret_cast<char *>(alloc) - headerAdjustment, *reinterpret_cast<Size *>(reinterpret_cast<char *>(alloc) + sizeof(Allocator *))});
        }
    }
}

#endif //STICK_ALLOCATOR_HPP
