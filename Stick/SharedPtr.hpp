#ifndef STICK_SHAREDPTR_HPP
#define STICK_SHAREDPTR_HPP

#include <Stick/DefaultCleanup.hpp>
#include <type_traits>

namespace stick
{
    namespace detail
    {
        template<class C>
        struct STICK_API ControlBlock
        {
            inline ControlBlock(Allocator & _alloc, C _cleanup) :
                count(1),
                cleanup(_alloc),
                allocator(&_alloc)
            {

            }

            inline void increment()
            {
                ++count;
            }

            inline void decrement()
            {
                --count;
            }

            Size count;
            C cleanup;
            Allocator * allocator; //allocator that allocated the control block
        };
    }

    template<class T, class C = DefaultCleanup<T>>
    class STICK_API SharedPtr
    {
        using Cleanup = C;
        using ControlBlock = detail::ControlBlock<C>;

    public:

        static_assert(!std::is_array<T>::value, "Array Types are not supported.");

        using ValueType = T;

        SharedPtr() :
            m_controlBlock(nullptr),
            m_ptr(nullptr)
        {

        }

        template<class U>
        explicit SharedPtr(U * _ptr, Cleanup _cleanup = Cleanup(defaultAllocator())) :
            m_controlBlock(defaultAllocator().create<ControlBlock>(defaultAllocator(), _cleanup)),
            m_ptr(_ptr)
        {
        }

        template<class U>
        SharedPtr(const SharedPtr<U> & _other) :
            m_controlBlock(_other.m_controlBlock),
            m_ptr(_other.m_ptr)
        {
            if (m_controlBlock)
                m_controlBlock->increment();
        }

        template<class U>
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
                    m_controlBlock->cleanup(m_ptr);
                    m_controlBlock->allocator->destroy(m_controlBlock);
                    m_controlBlock = nullptr;
                    m_ptr = nullptr;
                }
            }
        }

        Size useCount() const
        {
            return m_controlBlock ? m_controlBlock->count : 0;
        }

        SharedPtr & operator = (const SharedPtr & _other)
        {
            m_ptr = _other.m_ptr;
            m_controlBlock = _other.m_controlBlock;
            if (m_controlBlock)
                m_controlBlock->increment();
            return *this;
        }

        SharedPtr & operator = (SharedPtr && _other)
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

        ControlBlock * m_controlBlock;
        T * m_ptr;
    };
}

#endif //STICK_SHAREDPTR_HPP
