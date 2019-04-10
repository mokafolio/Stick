#ifndef STICK_SHAREDPTR_HPP
#define STICK_SHAREDPTR_HPP

#include <Stick/DefaultCleanup.hpp>
#include <type_traits>

namespace stick
{
namespace detail
{
struct STICK_API ControlBlock
{
    ControlBlock(Allocator & _alloc) : count(1), allocator(&_alloc)
    {
    }

    virtual ~ControlBlock()
    {
        STICK_ASSERT(count == 0);
    }

    virtual void destroyObj() = 0;

    void increment()
    {
        ++count;
    }

    void decrement()
    {
        --count;
    }

    Size count;
    Allocator * allocator; // allocator that allocated the control block
};

template <class T, class C>
struct STICK_API ControlBlockT : public ControlBlock
{
    ControlBlockT(Allocator & _alloc, T * _ptr, C _cleanup) :
        ControlBlock(_alloc),
        cleanup(_alloc),
        ptr(_ptr)
    {
    }

    void destroyObj() final
    {
        cleanup(ptr);
    }

    C cleanup;
    T * ptr;
};
} // namespace detail

template <class T, class C = DefaultCleanup<T>>
class STICK_API SharedPtr
{
    template <class T2, class C2>
    friend class SharedPtr;

    using Cleanup = C;
    using ControlBlockType = detail::ControlBlockT<T, C>;

  public:
    static_assert(!std::is_array<T>::value, "Array Types are not supported.");

    using ValueType = T;

    SharedPtr() : m_controlBlock(nullptr), m_ptr(nullptr)
    {
    }

    template <class U,
              class Enable = typename std::enable_if<std::is_convertible<U *, T *>::value>::type>
    explicit SharedPtr(U * _ptr, Cleanup _cleanup = Cleanup(defaultAllocator())) :
        m_controlBlock(
            defaultAllocator().create<ControlBlockType>(defaultAllocator(), _ptr, _cleanup)),
        m_ptr(static_cast<T *>(_ptr))
    {
    }

    SharedPtr(const SharedPtr & _other) : m_controlBlock(_other.m_controlBlock), m_ptr(_other.m_ptr)
    {
        if (m_controlBlock)
            m_controlBlock->increment();
    }

    template <class U,
              class Enable = typename std::enable_if<std::is_convertible<U *, T *>::value>::type>
    SharedPtr(const SharedPtr<U> & _other) :
        m_controlBlock(_other.m_controlBlock),
        m_ptr(static_cast<T *>(_other.m_ptr))
    {
        if (m_controlBlock)
            m_controlBlock->increment();
    }

    template <class U>
    SharedPtr(const SharedPtr<U> & _other, ValueType * _ptr) :
        m_controlBlock(_other.m_controlBlock),
        m_ptr(_ptr)
    {
        if (m_controlBlock)
            m_controlBlock->increment();
    }

    template <class U>
    SharedPtr(SharedPtr<U> && _other) :
        m_controlBlock(std::move(_other.m_controlBlock)),
        m_ptr(std::move(_other.m_ptr))
    {
        _other.m_controlBlock = nullptr;
        _other.m_ptr = nullptr;
    }

    ~SharedPtr()
    {
        reset();
    }

    void reset()
    {
        if (m_controlBlock)
        {
            m_controlBlock->decrement();
            if (m_controlBlock->count == 0)
            {
                m_controlBlock->destroyObj();
                Allocator * alloc = m_controlBlock->allocator;
                alloc->destroy(m_controlBlock);
            }

            m_controlBlock = nullptr;
            m_ptr = nullptr;
        }
    }

    Size useCount() const
    {
        return m_controlBlock ? m_controlBlock->count : 0;
    }

    SharedPtr & operator=(const SharedPtr & _other)
    {
        m_ptr = _other.m_ptr;
        m_controlBlock = _other.m_controlBlock;
        if (m_controlBlock)
            m_controlBlock->increment();
        return *this;
    }

    SharedPtr & operator=(SharedPtr && _other)
    {
        m_ptr = std::move(_other.m_ptr);
        m_controlBlock = std::move(_other.m_controlBlock);
        _other.m_controlBlock = nullptr;
        _other.m_ptr = nullptr;
        return *this;
    }

    void swap(SharedPtr & _other)
    {
        std::swap(m_controlBlock, _other.m_controlBlock);
        std::swap(m_ptr, _other.m_ptr);
    }

    T * get() const
    {
        STICK_ASSERT(m_controlBlock && m_ptr);
        return m_ptr;
    }

    T * operator->()
    {
        STICK_ASSERT(m_controlBlock && m_ptr);
        return m_ptr;
    }

    const T * operator->() const
    {
        STICK_ASSERT(m_controlBlock && m_ptr);
        return m_ptr;
    }

    T & operator*()
    {
        STICK_ASSERT(m_controlBlock && m_ptr);
        return *m_ptr;
    }

    const T & operator*() const
    {
        STICK_ASSERT(m_controlBlock && m_ptr);
        return *m_ptr;
    }

    explicit operator bool() const
    {
        return m_ptr;
    }

  private:
    detail::ControlBlock * m_controlBlock;
    T * m_ptr;
};

//@TODO make extra hidden constructor for SharedPtr that only performs one allocation
template <class T, class... Args>
SharedPtr<T> makeShared(Args &&... _args)
{
    return SharedPtr<T>(defaultAllocator().create<T>(std::forward<Args>(_args)...));
}

template <class T, class... Args>
SharedPtr<T> makeShared(Allocator & _alloc, Args &&... _args)
{
    return SharedPtr<T>(_alloc.create<T>(std::forward<Args>(_args)...), DefaultCleanup<T>(_alloc));
}

} // namespace stick

#endif // STICK_SHAREDPTR_HPP
