#ifndef STICK_ALLOCATORS_SEGREGATOR_HPP
#define STICK_ALLOCATORS_SEGREGATOR_HPP

namespace stick
{
    namespace mem
    {
        template<Size Threshold, class Small, class Large>
        class STICK_API Segregator
        {
        public:

            using SmallAllocator = Small;
            using LargeAllocator = Large;

            static constexpr Size alignment = (Small::alignment > Large::alignment) ?
                                              Small::alignment : Large::alignment;


            inline bool owns(const Block & _blk)
            {
                if (_blk.size <= Threshold)
                    return m_small.owns(_blk);
                else
                    return m_large.owns(_blk);
            }

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                STICK_ASSERT(_byteCount);
                if (_byteCount <= Threshold)
                    return m_small.allocate(_byteCount, _alignment);
                else
                    return m_large.allocate(_byteCount, _alignment);
            }

            inline void deallocate(const Block & _blk)
            {
                if (_blk.size <= Threshold)
                    m_small.deallocate(_blk);
                else
                    m_large.deallocate(_blk);
            }

        private:

            SmallAllocator m_small;
            LargeAllocator m_large;
        };
    }
}

#endif //STICK_ALLOCATORS_SEGREGATOR_HPP
