#ifndef STICK_ALLOCATORS_BUCKETIZER_HPP
#define STICK_ALLOCATORS_BUCKETIZER_HPP

namespace stick
{
    namespace mem
    {
        template<class Alloc, Size MinSize, Size MaxSize, Size StepSize>
        class STICK_API Bucketizer
        {
        public:

            using ParentAllocator = typename Alloc::Allocator;

            static constexpr Size alignment = Alloc::alignment;

            static constexpr Size bucketCount = ((MaxSize - MinSize + 1) / StepSize);


            

        private:

            Alloc m_allocators[bucketCount];
        };
    }
}

#endif //STICK_ALLOCATORS_BUCKETIZER_HPP
