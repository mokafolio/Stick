#ifndef STICK_UNIQUEPTR_HPP
#define STICK_UNIQUEPTR_HPP

#include <Stick/DefaultCleanup.hpp>
#include <cstddef> //for std::nullptr_t
#include <type_traits>

namespace stick
{
    template<class T, class C = DefaultCleanup<T>>
    class UniquePtr
    {
        static_assert(!std::is_array<T>::value, "Array Types are not supported.");

        template<class T2, class C2>
        friend class UniquePtr;

    public:

        typedef T ValueType;
        typedef T & ReferenceType;
        typedef T * PointerType;
        typedef C Cleanup;

        inline UniquePtr() :
            m_obj(nullptr)
        {

        }

        // template<class O>
        // inline UniquePtr(O * _ptr, Allocator & _alloc = defaultAllocator()) :
        //     m_obj(_ptr),
        //     m_cleanup(_alloc)
        // {

        // }

        template<class O>
        inline UniquePtr(O * _ptr, Cleanup _c = Cleanup(defaultAllocator())) :
            m_obj(_ptr),
            m_cleanup(_c)
        {

        }

        template<class O, class C2>
        inline UniquePtr(UniquePtr<O, C2> && _other) :
            m_obj(std::move(_other.m_obj)),
            m_cleanup(std::move(_other.m_cleanup))
        {
            _other.m_obj = nullptr;
        }

        inline ~UniquePtr()
        {
            if (m_obj)
                m_cleanup(m_obj);
        }

        UniquePtr(const UniquePtr & _other) = delete;
        UniquePtr & operator = (const UniquePtr & _other) = delete;

        inline UniquePtr & operator=(std::nullptr_t)
        {
            reset();
            return *this;
        }

        inline UniquePtr & operator = (UniquePtr && _other)
        {
            reset();
            m_obj = std::move(_other.m_obj);
            m_cleanup = std::move(_other.m_cleanup);
            _other.m_obj = nullptr;
            return *this;
        }

        inline void reset()
        {
            if (m_obj)
            {
                m_cleanup(m_obj);
                m_obj = nullptr;
            }
        }

        inline PointerType release()
        {
            PointerType tmp = m_obj;
            m_obj = nullptr;
            return tmp;
        }

        inline PointerType operator -> () const
        {
            return m_obj;
        }

        inline ReferenceType operator * () const
        {
            return *m_obj;
        }

        inline explicit operator bool() const
        {
            return m_obj;
        }

        inline PointerType get() const
        {
            return m_obj;
        }

    private:

        PointerType m_obj;
        Cleanup m_cleanup;
    };

    template<class T, class...Args>
    inline UniquePtr<T, DefaultCleanup<T>> makeUnique(Allocator & _alloc, Args && ... _args)
    {
        using C = DefaultCleanup<T>;
        return UniquePtr<T, C>(_alloc.create<T>(std::forward<Args>(_args)...), C(_alloc));
    }

    template<class T, class...Args>
    inline UniquePtr<T, DefaultCleanup<T>> makeUnique(Args && ... _args)
    {
        using C = DefaultCleanup<T>;
        return UniquePtr<T, C>(defaultAllocator().create<T>(std::forward<Args>(_args)...), C(defaultAllocator()));
    }
}

#endif //STICK_UNIQUEPTR_HPP
