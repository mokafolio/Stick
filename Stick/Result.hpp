#ifndef STICK_STICKRESULT_HPP
#define STICK_STICKRESULT_HPP

#include <Stick/Maybe.hpp>
#include <Stick/Error.hpp>

// @TODO: Use Variant instead of Maybe (at least for the non reference version)

// TODO: make different specializations of Result and STICK_RESULT_HOLDER
// for references so that the default one can implement move semantics, too
#define STICK_RESULT_HOLDER(_name, _holderName) \
template<class T> \
struct _name \
{ \
    _name() \
    { \
 \
    } \
 \
    _name(const _name & _other) : \
    m_value(_other.m_value) \
    { \
 \
    } \
 \
    _name(_name && _other) : \
    m_value(std::move(_other.m_value)) \
    { \
 \
    } \
 \
    _name(const T & _data) : \
        m_value(_data) \
    { \
 \
    } \
 \
    _name(T && _data) : \
        m_value(std::move(_data)) \
    { \
 \
    } \
 \
    T & _holderName() \
    { \
        return *m_value; \
    } \
 \
    const T & _holderName() const \
    { \
        return *m_value; \
    } \
\
    T & get() \
    { \
        return *m_value; \
    } \
\
    const T & get() const \
    { \
        return *m_value; \
    } \
    stick::Maybe<T> m_value; \
};

namespace stick
{
    template<class T>
    class Result
    {
    public:

        inline Result()
        {

        }

        inline Result(const Result & _result) :
            m_error(_result.m_error),
            m_value(_result.m_value)
        {

        }

        inline Result(Result && _result) :
            m_error(std::move(_result.m_error)),
            m_value(std::move(_result.m_value))
        {

        }

        inline Result(const T & _result) :
            m_value(_result)
        {

        }

        inline Result(T && _result) :
            m_value(std::forward<T>(_result))
        {

        }

        inline Result(const Error & _error) :
            m_error(_error)
        {

        }

        inline Result & operator = (const Result & _result)
        {
            m_value = _result.m_value;
            m_error = _result.m_error;
            return *this;
        }

        inline Result & operator = (Result && _result)
        {
            m_value = std::move(_result.m_value);
            m_error = std::move(_result.m_error);
            return *this;
        }

        inline operator bool() const
        {
            return !static_cast<bool>(m_error);
        }

        inline const Error & error() const
        {
            return m_error;
        }

        inline void setError(const Error & _error)
        {
            m_error = _error;
        }

        inline void setError(Error && _error)
        {
            m_error = std::move(_error);
        }

        inline const T & ensure() const
        {
            if (!this->m_value)
            {
                printf("Called ensure on an empty result.\n");
                if (m_error)
                {
                    printf("The result holds the following error message: %s.\nThe generic error description is: %s.\n", m_error.message().cString(), m_error.description().cString());
                }
                exit(EXIT_FAILURE);
            }
            return *(this->m_value);
        }

        inline T & ensure()
        {
            return const_cast<T &>(const_cast<const Result &>(*this).ensure());
        }

        T & get()
        {
            return *m_value;
        }

        const T & get() const
        {
            return *m_value;
        }

    private:

        Error m_error;
        stick::Maybe<T> m_value;
    };

    template<class T>
    class Result<T &>
    {
    public:

        inline Result()
        {

        }

        inline Result(const Result & _result) :
            m_value(_result.m_value)
        {

        }

        inline Result(Result && _result) :
            m_value(std::move(_result.m_value))
        {

        }

        inline Result(T & _result) :
            m_value(_result)
        {

        }

        inline Result(const Error & _error) :
            m_error(_error)
        {

        }

        inline operator bool() const
        {
            return !static_cast<bool>(m_error);
        }

        inline const Error & error() const
        {
            return m_error;
        }

        inline void setError(const Error & _error)
        {
            m_error = _error;
        }

        inline void setError(Error && _error)
        {
            m_error = std::move(_error);
        }

        inline const T & ensure() const
        {
            if (!this->m_value)
            {
                printf("Called ensure on an empty result.\n");
                if (m_error)
                {
                    printf("The result holds the following error message: %s.\nThe generic error description is: %s.\n", m_error.message().cString(), m_error.description().cString());
                }
                exit(EXIT_FAILURE);
            }
            return *(this->m_value);
        }

        inline T & ensure()
        {
            return const_cast<T &>(const_cast<const Result &>(*this).ensure());
        }

        T & get()
        {
            return *m_value;
        }

        const T & get() const
        {
            return *m_value;
        }

    private:

        Error m_error;
        stick::Maybe<T &> m_value;
    };

    template<class T>
    inline constexpr Result<typename std::decay<T>::type> makeResult(T && _value)
    {
        return Result<typename std::decay<T>::type>(std::forward<T>(_value));
    }

    //STICK_RESULT_HOLDER(TextResultHolder, text);
    typedef Result<stick::String> TextResult;
}

#endif //STICK_STICKRESULT_HPP
