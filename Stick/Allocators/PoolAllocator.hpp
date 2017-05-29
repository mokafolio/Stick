#ifndef STICK_ALLOCATORS_POOLALLOCATOR_HPP
#define STICK_ALLOCATORS_POOLALLOCATOR_HPP

#include <Stick/Allocators/Block.hpp>
namespace stick
{
    namespace mem
    {
        template<class Alloc, Size MinSize, Size MaxSize, Size BucketCount>
        class STICK_API PoolAllocator
        {
        public:

            static constexpr Size alignment = Alloc::alignment;

            PoolAllocator(Alloc & _allocator) :
            m_parentAllocator(&_allocator)
            {
                Size size = MaxSize * BucketCount;
                m_memory = _allocator.allocate(size, alignment);
                deallocateAll();
            }

            ~PoolAllocator()
            {
                m_parentAllocator->deallocate(m_memory);
            }

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                if(_alignment != alignment || _byteCount > MaxSize || _byteCount < MinSize)
                    return {nullptr, 0};

                void * ret = m_freeList;
                m_freeList = m_freeList->next;

                return {ret, _byteCount};
            }

            inline bool owns(const Block & _blk) const
            {
                return _blk.size >= MinSize && _blk.size <= MaxSize;
            }

            inline void deallocate(const Block & _blk)
            {
                auto p = reinterpret_cast<Node*>(_blk.ptr);
                p->next = m_freeList;
                m_freeList = p;
            }

            inline void deallocateAll()
            {
                m_freeList = reinterpret_cast<Node *>(m_memory.ptr);

                // build the linked list of buckets
                Node * p = m_freeList;
                for (Size i = 0; i < BucketCount; ++i)
                {
                    p->next = reinterpret_cast<Node *>(reinterpret_cast<UPtr>(p) + MaxSize);
                    p = p->next;
                }

                p->next = nullptr;
            }

            inline const Block & block() const
            {
                return m_memory;
            }

        private:

            Alloc * m_parentAllocator;
            struct Node { Node * next; } * m_freeList;
            Block m_memory;
        };
    }
}

#endif //STICK_ALLOCATORS_POOLALLOCATOR_HPP
