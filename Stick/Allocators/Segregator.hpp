#ifndef STICK_ALLOCATORS_SEGREGATOR_HPP
#define STICK_ALLOCATORS_SEGREGATOR_HPP

#include <Stick/Allocators/Block.hpp>
#include <tuple>

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

            inline Size threshold() const
            {
                return Threshold;
            }

        private:

            SmallAllocator m_small;
            LargeAllocator m_large;
        };

        template<Size Threshold, class Alloc>
        struct Step
        {
            static constexpr Size threshold = Threshold;
            using Allocator = Alloc;
        };

        namespace detail
        {
            template <class... Args>
            struct TypeList
            {
                template <std::size_t N>
                using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
            };
        }

        template<Size S>
        struct Threshold
        {
            static constexpr Size size = S;
        };

        template<Size S>
        using T = Threshold<S>;

        template<class...>
        class STICK_API SegregatorGroup;

        template<class Allocator>
        class STICK_API SegregatorGroup<Allocator>
        {
        public:

            inline bool owns(const Block & _blk)
            {
                return m_alloc.owns(_blk);
            }

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                printf("LAST ALLOCATOR\n");
                return m_alloc.allocate(_byteCount, _alignment);
            }

            inline void deallocate(const Block & _blk)
            {
                m_alloc.deallocate(_blk);
            }

        private:

            Allocator m_alloc;
        };

        template<>
        class STICK_API SegregatorGroup<>
        {
        public:

            inline bool owns(const Block & _blk)
            {
                return false;
            }

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                return {nullptr, 0};
            }

            inline void deallocate(const Block & _blk)
            {
                //@TODO: Assert false?
            }
        };

        template<Size Threshold, class Alloc, class...Args>
        class STICK_API SegregatorGroup<T<Threshold>, Alloc, Args...>
        {
        public:

            inline bool owns(const Block & _blk)
            {
                if (_blk.size <= Threshold)
                    return m_small.owns(_blk);
                else
                    return m_large.owns(_blk);
            }

            inline Block allocate(Size _byteCount, Size _alignment)
            {
                printf("MY THRESH %lu %lu\n", _byteCount, Threshold);
                STICK_ASSERT(_byteCount);
                if (_byteCount <= Threshold)
                {
                    printf("DO DA ALLOCATE\n");
                    return m_small.allocate(_byteCount, _alignment);
                }
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

            Alloc m_small;
            SegregatorGroup<Args...> m_large;
        };
    }
}

#endif //STICK_ALLOCATORS_SEGREGATOR_HPP
