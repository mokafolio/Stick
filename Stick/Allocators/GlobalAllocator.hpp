#ifndef STICK_ALLOCATORS_GLOBALALLOCATOR_HPP
#define STICK_ALLOCATORS_GLOBALALLOCATOR_HPP

namespace stick
{
    namespace mem
    {
        template<class Alloc>
        class STICK_API GlobalAllocator
        {
        public:

            static constexpr Size alignment = Alloc::alignment;

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                return instance().allocate(_byteCount, _alignment);
            }

            inline bool owns(const Block & _blk)
            {
                return instance().owns(_blk);
            }

            inline void deallocate(const Block & _blk)
            {
                instance().deallocate(_blk);
            }

            inline Alloc & instance()
            {
                static Alloc s_instance;
                return s_instance;
            }
        };
    }
}

#endif //STICK_ALLOCATORS_GLOBALALLOCATOR_HPP
