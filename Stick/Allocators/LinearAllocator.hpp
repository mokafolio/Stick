#ifndef STICK_ALLOCATORS_LINEARALLOCATOR_HPP
#define STICK_ALLOCATORS_LINEARALLOCATOR_HPP

#include <Stick/Allocators/Block.hpp>
#include <Stick/Allocators/AllocatorUtilities.hpp>

namespace stick
{
    namespace mem
    {
        template<class Alloc, Size S>
        class STICK_API LinearAllocator
        {
        public:

            static constexpr Size alignment = Alloc::alignment;

            using ParentAllocator = Alloc;

            inline LinearAllocator()
            {
                m_memory = m_alloc.allocate(S, alignment);
                STICK_ASSERT(m_memory);
                m_position = m_memory.ptr;
            }

            inline ~LinearAllocator()
            {
                m_alloc.deallocate(m_memory);
            }

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                STICK_ASSERT(_byteCount > 0);

                Size adjustment = alignmentAdjustment(m_position, _alignment);

                if (m_memory.size - ((UPtr)m_position - (UPtr)m_memory.ptr) < adjustment + _byteCount)
                    return {nullptr, 0};

                void * alignedAddr = reinterpret_cast<void *>(reinterpret_cast<UPtr>(m_position) + adjustment);
                m_position = reinterpret_cast<void *>(reinterpret_cast<UPtr>(alignedAddr) + _byteCount);
                return {alignedAddr, _byteCount};
            }

            inline bool owns(const Block & _blk)
            {
                // @TODO: Also check if _blk.ptr is smaller then m_position?
                return _blk.ptr >= m_memory.ptr && reinterpret_cast<UPtr>(_blk.ptr) <= reinterpret_cast<UPtr>(m_memory.ptr) + m_memory.size;
            }

            inline void deallocate(const Block & _blk)
            {
                // if this is the last element we allocated, we can deallocate it
                void * tmp = reinterpret_cast<void *>(reinterpret_cast<UPtr>(m_position) - _blk.size);
                if (tmp == _blk.ptr)
                    m_position = tmp;

                //... otherwise we can't :(
            }

            inline void deallocateAll()
            {
                m_position = m_memory.ptr;
            }

            inline const Block & block() const
            {
                return m_memory;
            }

            inline void * currentPosition() const
            {
                return m_position;
            }

        private:

            ParentAllocator m_alloc;
            Block m_memory;
            void * m_position;
        };
    }
}

#endif //STICK_ALLOCATORS_LINEARALLOCATOR_HPP
