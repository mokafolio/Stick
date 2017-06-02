#ifndef STICK_ALLOCATORS_FALLBACKALLOCATOR_HPP
#define STICK_ALLOCATORS_FALLBACKALLOCATOR_HPP

#include <Stick/Allocators/Block.hpp>

namespace stick
{
    namespace mem
    {
        template<class P, class F>
        class STICK_API FallbackAllocator
        {
        public:

            using Primary = P;
            using Fallback = F;

            static constexpr Size alignment = (Primary::alignment > Fallback::alignment) ?
                                              Primary::alignment : Fallback::alignment;

            inline bool owns(const Block & _blk)
            {
                if(m_primary.owns(_blk))
                    return true;
                return m_fallback.owns(_blk);
            }

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                STICK_ASSERT(_byteCount);
                Block ret = m_primary.allocate(_byteCount, _alignment);
                if(ret) return ret;
                return m_fallback.allocate(_byteCount, _alignment);
            }

            inline void deallocate(const Block & _blk)
            {
                if(m_primary.owns(_blk))
                    m_primary.deallocate(_blk);
                else
                    m_fallback.deallocate(_blk);
            }

        private:

            Primary m_primary;
            Fallback m_fallback;
        };
    }
}

#endif //STICK_ALLOCATORS_FALLBACKALLOCATOR_HPP
