#ifndef STICK_ALLOCATOR_HPP
#define STICK_ALLOCATOR_HPP

#include <Stick/Allocators/Mallocator.hpp>
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
    static DefaultAllocator m_def;
    return m_def;
}
} // namespace stick

#endif // STICK_ALLOCATOR_HPP
