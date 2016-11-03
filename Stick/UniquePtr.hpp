#ifndef STICK_UNIQUEPTR_HPP
#define STICK_UNIQUEPTR_HPP

#include <Stick/Allocator.hpp>
#include <cstddef> //for std::nullptr_t

namespace stick
{
    template<class T>
    class DefaultCleanup
    {
    public:

        inline DefaultCleanup(Allocator & _alloc = defaultAllocator()) :
            m_alloc(&_alloc)
        {

        }

        inline void operator() (T * _obj) const
        {
            destroy(_obj, *m_alloc);
        }

    private:

        Allocator * m_alloc;
    };

    template<class T, template<class> class C = DefaultCleanup>
    class UniquePtr
    {
    public:

        typedef T ValueType;
        typedef T & ReferenceType;
        typedef T * PointerType;
        typedef C<T> Cleanup;

        inline UniquePtr() :
            m_obj(nullptr)
        {

        }

        inline UniquePtr(PointerType _ptr, const Cleanup & _c = Cleanup()) :
            m_obj(_ptr),
            m_cleanup(_c)
        {

        }

        inline UniquePtr(UniquePtr && _other) :
            m_obj(std::move(_other.m_obj)),
            m_cleanup(std::move(_other.m_cleanup))
        {
            printf("MOVE\n");
            _other.m_obj = nullptr;
        }

        inline ~UniquePtr()
        {
            printf("~UniquePtr\n");
            if (m_obj)
            {
                printf("DELETE OBJ\n");
                m_cleanup(m_obj);
            }
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
            printf("RESET\n");
            if (m_obj)
            {
                printf("DELETE OBJ\n");
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
}

#endif //STICK_UNIQUEPTR_HPP
