#ifndef STICK_ALLOCATORS_POOLALLOCATOR_HPP
#define STICK_ALLOCATORS_POOLALLOCATOR_HPP

#include <Stick/Allocators/MemoryChunk.hpp>

namespace stick
{
    namespace mem
    {
        namespace detail
        {
            template<Size S>
            struct DynamicSizeHelper
            {
                Size size() const
                {
                    return S;
                }
            };

            enum DynamicSizeFlags : Size
            {
                Undefined = std::numeric_limits<Size>::max() - 1,
                Dynamic = std::numeric_limits<Size>::max()
            };

            template<>
            struct DynamicSizeHelper<Dynamic>
            {
                DynamicSizeHelper() :
                    value(Undefined)
                {
                }

                Size size() const
                {
                    return value;
                }

                void set(Size _val)
                {
                    value = _val;
                }

                Size value;
            };
        }

        static constexpr Size DynamicSizeFlag = detail::Dynamic;

        template<class Alloc, Size MinSize, Size MaxSize, Size BucketCount>
        class STICK_API PoolAllocator
        {
        public:

            static constexpr Size alignment = Alloc::alignment;

            using ParentAllocator = Alloc;

            inline PoolAllocator() :
                m_freeList(nullptr)
            {
                // if (m_min.size() != detail::Undefined && m_max.size() != detail::Undefined)
                // {
                //     initialize();
                // }
            }

            inline PoolAllocator(Size _min, Size _max) :
                m_freeList(nullptr)
            {
                setMinMax(_min, _max);
            }

            inline ~PoolAllocator()
            {
                // iterate over all allocater blocks and deallocate them
                if (m_firstBlock.memory)
                {
                    static constexpr Size headerSize = sizeof(MemoryChunk);
                    static constexpr Size headerAdjustment = headerSize % alignment == 0 ? headerSize : headerSize + headerSize + alignment - headerSize % alignment;
                    MemoryChunk * pb = &m_firstBlock;
                    while (pb)
                    {
                        m_alloc.deallocate({(void *)((UPtr)pb->memory.ptr - headerAdjustment), pb->memory.size + headerAdjustment});
                        pb = pb->next;
                    }
                }
            }

            inline void setMinMax(Size _min, Size _max)
            {
                //this function should only be called once
                STICK_ASSERT(m_min.size() == detail::Undefined);
                STICK_ASSERT(m_max.size() == detail::Undefined);

                m_min.set(_min);
                m_max.set(_max);

                // initialize();
            }

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                if (_byteCount <= m_max.size() &&
                        _byteCount >= m_min.size())
                {
                    if (!m_freeList)
                    {
                        allocateChunk();
                    }

                    if(reinterpret_cast<UPtr>(m_freeList) % _alignment != 0)
                        return {nullptr, 0};

                    void * ret = m_freeList;
                    m_freeList = m_freeList->next;
                    return {ret, _byteCount};
                }

                return {nullptr, 0};
            }

            inline bool owns(const Block & _blk) const
            {
                const MemoryChunk * pb = &m_firstBlock;
                while (pb)
                {
                    auto ret = pb->owns(_blk);
                    if (ret) return ret;
                    pb = pb->next;
                }
                return false;
            }

            inline void deallocate(const Block & _blk)
            {
                printf("DEALLOC\n");
                STICK_ASSERT(m_firstBlock.memory);
                STICK_ASSERT(owns(_blk));
                printf("DEALLOC 2\n");
                auto p = reinterpret_cast<Node *>(_blk.ptr);
                p->next = m_freeList;
                m_freeList = p;
            }

            inline void deallocateAll()
            {
                // m_freeList = reinterpret_cast<Node *>(m_memory.ptr);

                // // build the linked list of buckets
                // Node * p = m_freeList;
                // for (Size i = 1; i < BucketCount; ++i)
                // {
                //     UPtr ptr = reinterpret_cast<UPtr>(p) + m_max.size();
                //     p->next = reinterpret_cast<Node *>(ptr);
                //     p = p->next;
                // }

                // p->next = nullptr;
                // m_firstBlock.deallocateAll(m_max.size(), BucketCount);
                MemoryChunk * pb = &m_firstBlock;
                m_freeList = reinterpret_cast<Node *>(m_firstBlock.memory.ptr);
                Node * p = m_freeList;
                while (pb)
                {
                    Node * p2 = reinterpret_cast<Node *>(pb->memory.ptr);
                    if (pb != &m_firstBlock)
                    {
                        p->next = p2;
                        p = p2;
                    }
                    for (Size i = 1; i < BucketCount; ++i)
                    {
                        UPtr ptr = reinterpret_cast<UPtr>(p2) + m_max.size();
                        p->next = reinterpret_cast<Node *>(ptr);
                        p = p->next;
                    }
                    pb = pb->next;
                }
            }

            inline Size min() const
            {
                return m_min.size();
            }

            inline Size max() const
            {
                return m_max.size();
            }

            inline Size freeCount() const
            {
                Size ret = 0;
                Node * p = m_freeList;
                while (p)
                {
                    p = p->next;
                    ret++;
                }
                return ret;
            }

            inline Size chunkCount() const
            {
                Size ret = 0;
                auto * p = &m_firstBlock;
                while (p)
                {
                    p = p->next;
                    ret++;
                }
                return ret;
            }

        private:

            // void initialize()
            // {
            //     // STICK_ASSERT(m_max.size() % alignment == 0);
            //     // m_freeList = nullptr;
            //     // Size size = m_max.size() * BucketCount;
            //     // m_memory = m_alloc.allocate(size, alignment);
            //     // STICK_ASSERT(m_memory);
            //     // deallocateAll();

            //     static constexpr Size headerSize = sizeof(MemoryChunk);
            //     Size size = m_max.size() * BucketCount + headerSize;
            //     Block mem = m_alloc.allocate(size, alignment);
            //     m_firstBlock = MemoryChunk({(void *)((UPtr)mem.ptr + headerSize), mem.size - BucketCount}, m_max.size(), BucketCount);
            // }

            void allocateChunk()
            {
                static constexpr Size headerSize = sizeof(MemoryChunk);
                static constexpr Size headerAdjustment = headerSize % alignment == 0 ? headerSize : headerSize + headerSize + alignment - headerSize % alignment;

                Size size = m_max.size() * BucketCount + headerAdjustment;
                Block mem = m_alloc.allocate(size, alignment);
                MemoryChunk blk({(void *)((UPtr)mem.ptr + headerAdjustment), mem.size - headerAdjustment});
                if (!m_firstBlock.memory)
                {
                    m_firstBlock = std::move(blk);
                }
                else
                {
                    m_firstBlock.next = reinterpret_cast<MemoryChunk *>((UPtr)blk.memory.ptr - headerAdjustment);
                    *m_firstBlock.next = std::move(blk);
                }

                // build the linked list of buckets
                Node * p = reinterpret_cast<Node *>(blk.memory.ptr);
                Node * first = p;
                for (Size i = 1; i < BucketCount; ++i)
                {
                    UPtr ptr = reinterpret_cast<UPtr>(p) + m_max.size();
                    p->next = reinterpret_cast<Node *>(ptr);
                    p = p->next;
                }

                p->next = nullptr;

                //@TODO: I think its fair to assume that this will always be a fresh freelist
                // as we only call this function when we are out of free buckets. Safety first for now though i guess.
                if (!m_freeList)
                {
                    m_freeList = first;
                }
                else
                {
                    p->next = m_freeList;
                    m_freeList = first;
                }
            }

            ParentAllocator m_alloc;
            detail::DynamicSizeHelper<MinSize> m_min;
            detail::DynamicSizeHelper<MaxSize> m_max;
            MemoryChunk m_firstBlock;
            struct Node { Node * next; };
            Node * m_freeList;
        };
    }
}

#endif //STICK_ALLOCATORS_POOLALLOCATOR_HPP
