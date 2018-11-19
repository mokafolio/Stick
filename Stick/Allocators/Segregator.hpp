#ifndef STICK_ALLOCATORS_SEGREGATOR_HPP
#define STICK_ALLOCATORS_SEGREGATOR_HPP

#include <Stick/Allocators/Block.hpp>
#include <tuple>

namespace stick
{
namespace mem
{
template <Size S>
struct Threshold
{
    static constexpr Size size = S;
};

template <Size S>
using T = Threshold<S>;

template <class...>
class STICK_API Segregator;

template <class Allocator>
class STICK_API Segregator<Allocator>
{
  public:
    static constexpr Size alignment = Allocator::alignment;

    inline bool owns(const Block & _blk)
    {
        return m_alloc.owns(_blk);
    }

    inline Block allocate(Size _byteCount, Size _alignment)
    {
        return m_alloc.allocate(_byteCount, _alignment);
    }

    inline void deallocate(const Block & _blk)
    {
        m_alloc.deallocate(_blk);
    }

  private:
    Allocator m_alloc;
};

template <>
class STICK_API Segregator<>
{
  public:
    static constexpr Size alignment = -1;

    inline bool owns(const Block & _blk)
    {
        return false;
    }

    inline Block allocate(Size _byteCount, Size _alignment)
    {
        return { nullptr, 0 };
    }

    inline void deallocate(const Block & _blk)
    {
        //@TODO: Assert false?
    }
};

template <Size Threshold, class Alloc, class... Args>
class STICK_API Segregator<T<Threshold>, Alloc, Args...>
{
  public:
    using SmallAllocator = Alloc;
    using LargeAllocator = Segregator<Args...>;

    static constexpr Size alignment = (SmallAllocator::alignment > LargeAllocator::alignment)
                                          ? SmallAllocator::alignment
                                          : LargeAllocator::alignment;

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
        {
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

    inline const SmallAllocator & smallAllocator() const
    {
        return m_small;
    }

    inline const LargeAllocator & largeAllocator() const
    {
        return m_large;
    }

    inline const LargeAllocator & nextAllocator() const
    {
        return m_large;
    }

  private:
    SmallAllocator m_small;
    LargeAllocator m_large;
};
} // namespace mem
} // namespace stick

#endif // STICK_ALLOCATORS_SEGREGATOR_HPP
