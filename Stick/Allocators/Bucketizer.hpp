#ifndef STICK_ALLOCATORS_BUCKETIZER_HPP
#define STICK_ALLOCATORS_BUCKETIZER_HPP

#include <Stick/Allocators/Block.hpp>

namespace stick
{
    namespace mem
    {
        template<class Alloc, Size MinSize, Size MaxSize, Size StepSize>
        class STICK_API Bucketizer
        {
        public:

            static constexpr Size alignment = Alloc::alignment;

            static constexpr Size bucketCount = ((MaxSize - MinSize + 1) / StepSize);


            inline Bucketizer()
            {
                for (Size i = 0; i < bucketCount; ++i)
                {
                    m_allocators[i].setMinMax(MinSize + i * StepSize, MinSize + (i + 1) * StepSize - 1);
                }
            }

            inline bool owns(const Block & _blk) const
            {
                //TODO: Forward to the actual buckets?
                return _blk.size >= MinSize && _blk.size <= MaxSize;
            }

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                Alloc * a = findAllocator(_byteCount);
                if (a)
                {
                    return a->allocate(_byteCount, _alignment);
                }
                return {nullptr, 0};
            }

            inline void deallocate(const Block & _blk)
            {
                STICK_ASSERT(owns(_blk));
                findAllocator(_blk.size)->deallocate(_blk);
            }

        private:

            inline Alloc * findAllocator(Size _s) 
            {
                if (_s >= MinSize && _s <= MaxSize)
                {
                    auto idx = (_s - MinSize) / StepSize;
                    return &m_allocators[idx];
                }
                return nullptr;
            }

            Alloc m_allocators[bucketCount];
        };
    }
}

#endif //STICK_ALLOCATORS_BUCKETIZER_HPP
