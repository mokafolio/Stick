#ifndef STICK_ALLOCATORS_POOLALLOCATOR_HPP
#define STICK_ALLOCATORS_POOLALLOCATOR_HPP

#include <Stick/Allocators/Block.hpp>

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

            inline PoolAllocator()
            {
                if (m_min.size() != detail::Undefined && m_max.size() != detail::Undefined)
                {
                    printf("CALL INIT\n");
                    initialize();
                }
            }

            inline PoolAllocator(Size _min, Size _max)
            {
                setMinMax(_min, _max);
            }

            inline ~PoolAllocator()
            {
                m_alloc.deallocate(m_memory);
            }

            inline void setMinMax(Size _min, Size _max)
            {
                //this function should only be called once
                STICK_ASSERT(!m_memory);
                STICK_ASSERT(m_min.size() == detail::Undefined);
                STICK_ASSERT(m_max.size() == detail::Undefined);

                m_min.set(_min);
                m_max.set(_max);

                initialize();
            }

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                if (_alignment != alignment || _byteCount > MaxSize || _byteCount < MinSize)
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
                STICK_ASSERT(owns(_blk));
                auto p = reinterpret_cast<Node *>(_blk.ptr);
                p->next = m_freeList;
                m_freeList = p;
            }

            inline void deallocateAll()
            {
                m_freeList = reinterpret_cast<Node *>(m_memory.ptr);

                // build the linked list of buckets
                Node * p = m_freeList;
                for (Size i = 1; i < BucketCount; ++i)
                {
                    UPtr ptr = reinterpret_cast<UPtr>(p) + m_max.size();
                    p->next = reinterpret_cast<Node *>(ptr);
                    p = p->next;
                }

                p->next = nullptr;
            }

            inline const Block & block() const
            {
                return m_memory;
            }

        private:

            void initialize()
            {
                Size size = m_max.size() * BucketCount;
                m_memory = m_alloc.allocate(size, alignment);
                STICK_ASSERT(m_memory);
                deallocateAll();
            }

            ParentAllocator m_alloc;
            struct Node { Node * next; } * m_freeList;
            Block m_memory;
            detail::DynamicSizeHelper<MinSize> m_min;
            detail::DynamicSizeHelper<MaxSize> m_max;
        };
    }
}

#endif //STICK_ALLOCATORS_POOLALLOCATOR_HPP
