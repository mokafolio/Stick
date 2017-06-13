#ifndef STICK_ALLOCATORS_MEMORYCHUNK_HPP
#define STICK_ALLOCATORS_MEMORYCHUNK_HPP

#include <Stick/Allocators/Block.hpp>

namespace stick
{
    namespace mem
    {
        struct STICK_API MemoryChunk
        {
            inline MemoryChunk() :
                next(nullptr)
            {

            }

            inline MemoryChunk(const Block & _blk) :
                next(nullptr),
                memory(_blk)
            {
                STICK_ASSERT(memory);
            }

            MemoryChunk(const MemoryChunk & ) = default;
            MemoryChunk(MemoryChunk &&) = default;
            MemoryChunk & operator = (const MemoryChunk &) = default;
            MemoryChunk & operator = (MemoryChunk &&) = default;

            inline bool owns(const Block & _blk) const
            {
                STICK_ASSERT(memory);
                return _blk.ptr >= memory.ptr && reinterpret_cast<UPtr>(_blk.ptr) <= reinterpret_cast<UPtr>(memory.ptr) + memory.size;
            }

            inline MemoryChunk * lastChunk()
            {
                MemoryChunk * ret = this;
                while(ret->next)
                {
                    ret = ret->next;
                }
                return ret;
            }

            Block memory;
            MemoryChunk * next;
        };
    }
}

#endif //STICK_ALLOCATORS_MEMORYCHUNK_HPP
