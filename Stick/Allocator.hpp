#ifndef STICK_ALLOCATOR_HPP
#define STICK_ALLOCATOR_HPP

#include <Stick/Allocators/Mallocator.hpp>
#include <Stick/Allocators/Segregator.hpp>
#include <Stick/Allocators/PoolAllocator.hpp>
#include <Stick/Allocators/Bucketizer.hpp>
#include <Stick/Utility.hpp>
#include <algorithm>
#include <stdlib.h>

namespace stick
{
class STICK_API Allocator
{
  public:
    virtual ~Allocator() = default;

    virtual mem::Block allocate(Size _byteCount, Size _alignment) = 0;

    virtual void deallocate(const mem::Block & _block) = 0;

    //@TODO: replace modulo with faster ways to align
    // on the other hand its static so maybe not worth it :)
    template <class T, class... Args>
    inline T * create(Args &&... _args)
    {
        static constexpr auto headerSize = sizeof(Size);
        static constexpr auto headerAdjustment =
            headerSize % alignof(T) == 0 ? 0 : headerSize + alignof(T) - headerSize % alignof(T);
        static constexpr auto totalSize = sizeof(T) + headerSize + headerAdjustment;

        auto mem = allocate(totalSize, alignof(T));
        UPtr addr = reinterpret_cast<UPtr>(mem.ptr) + headerAdjustment;
        Size * ptr = reinterpret_cast<Size *>(addr);
        *ptr = totalSize;
        return new (reinterpret_cast<void *>(addr + headerSize)) T(std::forward<Args>(_args)...);
    }

    template <class T>
    inline void destroy(T * _obj)
    {
        if (_obj)
        {
            static constexpr auto headerSize = sizeof(Size);
            static constexpr auto headerAdjustment =
                headerSize % alignof(T) == 0 ? 0
                                             : headerSize + alignof(T) - headerSize % alignof(T);
            void * ptr = reinterpret_cast<void *>(reinterpret_cast<UPtr>(_obj) - headerSize -
                                                  headerAdjustment);
            _obj->~T();
            deallocate({ ptr, *reinterpret_cast<Size *>(ptr) });
        }
    }
};

class STICK_API DefaultAllocator : public Allocator
{
  public:
    DefaultAllocator()
    {
    }

    ~DefaultAllocator()
    {
    }

    inline mem::Block allocate(Size _byteCount, Size _alignment) override
    {
        return m_alloc.allocate(_byteCount, _alignment);
    }

    inline void deallocate(const mem::Block & _block) override
    {
        m_alloc.deallocate(_block);
    }

  private:
    mem::Mallocator m_alloc;
};

inline STICK_API Allocator & defaultAllocator()
{
    // Note: This is rather ugly for now. Since we want the default allocator to be guaranteed to
    // outlive any other object, we lazily heap allocate it (and never destruct it). Otherwise
    // random destruction order across multiple translation units might bite us in the butt.
    static DefaultAllocator * m_def = new DefaultAllocator;
    return *m_def;
}

class STICK_API ExperimentalAllocator : public Allocator
{
  public:
    ExperimentalAllocator()
    {
    }

    ~ExperimentalAllocator()
    {
    }

    inline mem::Block allocate(Size _byteCount, Size _alignment) override
    {
        return m_alloc.allocate(_byteCount, _alignment);
    }

    inline void deallocate(const mem::Block & _block) override
    {
        m_alloc.deallocate(_block);
    }

  private:
    using PoolAlloc =
        mem::PoolAllocator<mem::Mallocator, mem::DynamicSizeFlag, mem::DynamicSizeFlag, 1024>;
    using SmallAlloc = mem::PoolAllocator<mem::Mallocator, 0, 8, 1024>;
    using SegregatorAlloc = mem::Segregator<mem::T<8>,
                                            SmallAlloc,
                                            mem::T<128>,
                                            mem::Bucketizer<PoolAlloc, 1, 128, 16>,
                                            mem::T<256>,
                                            mem::Bucketizer<PoolAlloc, 129, 256, 32>,
                                            mem::T<512>,
                                            mem::Bucketizer<PoolAlloc, 257, 512, 64>,
                                            mem::T<1024>,
                                            mem::Bucketizer<PoolAlloc, 513, 1024, 128>,
                                            mem::T<2048>,
                                            mem::Bucketizer<PoolAlloc, 1025, 2048, 256>,
                                            mem::T<4096>,
                                            mem::Bucketizer<PoolAlloc, 2049, 4096, 512>,
                                            mem::Mallocator>;
    SegregatorAlloc m_alloc;
};

inline STICK_API Allocator & experimentalAllocator()
{
    // See note at defaultAllocator()
    static ExperimentalAllocator * m_def = new ExperimentalAllocator;
    return *m_def;
}

} // namespace stick

#endif // STICK_ALLOCATOR_HPP
