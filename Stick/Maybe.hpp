#ifndef STICK_MAYBE_HPP
#define STICK_MAYBE_HPP

#include <new>

namespace stick
{
    template<class T>
    class Maybe
    {
    public:

        typedef T ValueType;


        inline Maybe() :
            m_bHasValue(false)
        {

        }

        inline Maybe(const T & _value) :
            m_bHasValue(true)
        {
            new(m_data) T(_value);
        }

        inline Maybe(T && _value) :
            m_bHasValue(true)
        {
            new(m_data) T(move(_value));
        }

        inline Maybe(const Maybe & _other) :
            m_bHasValue(_other.m_bHasValue)
        {
            if (m_bHasValue)
                new(m_data) T(_other.value());
        }

        inline Maybe(Maybe && _other) :
            m_bHasValue(_other.m_bHasValue)
        {
            if (m_bHasValue)
                new(m_data) T(move(_other.value()));
        }

        inline ~Maybe()
        {
            if (m_bHasValue)
                value().~T();
        }

        inline T & value()
        {
            return *reinterpret_cast<T *>(m_data);
        }

        inline const T & value() const
        {
            return *reinterpret_cast<const T *>(m_data);
        }

        inline void reset()
        {
            if (m_bHasValue)
            {
                value().~T();
                m_bHasValue = false;
            }
        }

        /**
        * @brief Implicit conversion to bool.
        */
        inline explicit operator bool() const
        {
            return m_bHasValue;
        }

        template<class U>
        inline Maybe & operator = (U && _value)
        {
            if (m_bHasValue)
            {
                value() = forward<U>(_value);
            }
            else
            {
                new(m_data) T(forward<U>(_value));
                m_bHasValue = true;
            }
            return *this;
        }

        inline Maybe & operator = (const Maybe & _other)
        {
            if (_other.m_bHasValue)
            {
                if (m_bHasValue)
                    value() = _other.value();
                else
                {
                    new(m_data) T(_other.value());
                    m_bHasValue = true;
                }
            }
            else if (m_bHasValue)
            {
                reset();
            }
            return *this;
        }

        inline Maybe & operator = (Maybe && _other)
        {
            if (_other.m_bHasValue)
            {
                if (m_bHasValue)
                    value() = move(_other.value());
                else
                {
                    new(m_data) T(move(_other.value()));
                    m_bHasValue = true;
                }
            }
            else if (m_bHasValue)
            {
                reset();
            }
            return *this;
        }

        inline T * operator->()
        {
            return &value();
        }

        inline T * operator->() const
        {
            return &value();
        }

        inline T & operator*()
        {
            return value();
        }

        inline T & operator*() const
        {
            return value();
        }

        inline const T & ensure() const
        {
            if (!m_bHasValue)
            {
                printf("Called ensure on emtpy Maybe type.\n");
                exit(EXIT_FAILURE);
            }
            return value();
        }

        inline T & ensure()
        {
            return const_cast<T &>(const_cast<const Maybe &>(*this).ensure());
        }

    private:

        bool m_bHasValue;
        alignas(T) unsigned char m_data[sizeof(T)];
    };

    template<class T>
    class Maybe<T &>
    {
    public:

        typedef T ValueType;


        inline Maybe() :
            m_ptr(nullptr)
        {

        }

        inline Maybe(T & _value) :
            m_ptr(&_value)
        {
        }

        inline Maybe(const Maybe & _other) :
            m_ptr(_other.m_ptr)
        {
        }

        inline T & value()
        {
            return *m_ptr;
        }

        inline const T & value() const
        {
            return *m_ptr;
        }

        inline void reset()
        {
            m_ptr = nullptr;
        }

        /**
        * @brief Implicit conversion to bool.
        */
        inline explicit operator bool() const
        {
            return m_ptr;
        }

        inline Maybe & operator = (T & _value)
        {
            m_ptr = &_value;
            return *this;
        }

        inline Maybe & operator = (const Maybe & _other)
        {
            m_ptr = _other.m_ptr;
            return *this;
        }

        inline T & operator*()
        {
            return *m_ptr;
        }

        inline T & operator*() const
        {
            return *m_ptr;
        }

        inline const T & ensure() const
        {
            if (!m_ptr)
            {
                printf("Called ensure on emtpy Maybe type.\n");
                exit(EXIT_FAILURE);
            }
            return value();
        }

        inline T & ensure()
        {
            return const_cast<T &>(const_cast<const Maybe &>(*this).ensure());
        }

    private:

        T * m_ptr;
    };
}

#endif //STICK_MAYBE_HPP
