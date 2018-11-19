#ifndef STICK_RESULT_HPP
#define STICK_RESULT_HPP

#include <Stick/Error.hpp>
#include <Stick/Variant.hpp>

// // TODO: make different specializations of Result and STICK_RESULT_HOLDER
// // for references so that the default one can implement move semantics, too
// #define STICK_RESULT_HOLDER(_name, _holderName) \
// template<class T> \
// struct _name \
// { \
//     _name() \
//     { \
//  \
//     } \
//  \
//     _name(const _name & _other) : \
//     m_value(_other.m_value) \
//     { \
//  \
//     } \
//  \
//     _name(_name && _other) : \
//     m_value(std::move(_other.m_value)) \
//     { \
//  \
//     } \
//  \
//     _name(const T & _data) : \
//         m_value(_data) \
//     { \
//  \
//     } \
//  \
//     _name(T && _data) : \
//         m_value(std::move(_data)) \
//     { \
//  \
//     } \
//  \
//     T & _holderName() \
//     { \
//         return *m_value; \
//     } \
//  \
//     const T & _holderName() const \
//     { \
//         return *m_value; \
//     } \
// \
//     T & get() \
//     { \
//         return *m_value; \
//     } \
// \
//     const T & get() const \
//     { \
//         return *m_value; \
//     } \
//     stick::Maybe<T> m_value; \
// };

namespace stick
{
template <class T>
class Result
{
  public:
    inline Result()
    {
    }

    inline Result(const Result & _result) : m_variant(_result.m_variant)
    {
    }

    inline Result(Result && _result) : m_variant(std::move(_result.m_variant))
    {
    }

    inline Result(T & _result) : m_variant(_result)
    {
    }

    inline Result(T && _result) : m_variant(std::move(_result))
    {
    }

    inline Result(const Error & _error) : m_variant(_error)
    {
    }

    inline Result & operator=(const Result & _result)
    {
        m_variant = _result.m_variant;
        return *this;
    }

    inline Result & operator=(Result && _result)
    {
        m_variant = std::move(_result.m_variant);
        return *this;
    }

    inline operator bool() const
    {
        return m_variant.isValid() && !m_variant.template is<Error>();
    }

    inline const Error & error() const
    {
        STICK_ASSERT(m_variant.template is<Error>());
        return m_variant.template get<Error>();
        ;
    }

    inline void setError(const Error & _error)
    {
        m_variant = _error;
    }

    inline void setError(Error && _error)
    {
        m_variant = std::move(_error);
    }

    inline const T & ensure() const
    {
        if (!m_variant.isValid() || m_variant.template is<Error>())
        {
            printf("Called ensure on an empty result.\n");
            if (m_variant.template is<Error>())
            {
                printf(
                    "The result holds the following error message: %s.\nThe generic error "
                    "description is: %s.\n",
                    m_variant.template get<Error>().message().cString(),
                    m_variant.template get<Error>().description().cString());
            }
            exit(EXIT_FAILURE);
        }
        return m_variant.template get<T>();
    }

    inline T & ensure()
    {
        return const_cast<T &>(const_cast<const Result &>(*this).ensure());
    }

    /*
    inline T ensure() const
    {
        if (!m_variant.isValid() || m_variant.template  is<Error>())
        {
            printf("Called ensure on an empty result.\n");
            if (m_variant.template  is<Error>())
            {
                printf("The result holds the following error message: %s.\nThe generic error
    description is: %s.\n", m_variant.template get<Error>().message().cString(), m_variant.template
    get<Error>().description().cString());
            }
            exit(EXIT_FAILURE);
        }
        return m_variant.template get<T>();
    }*/

    T & get()
    {
        return m_variant.template get<T>();
    }

    const T & get() const
    {
        return m_variant.template get<T>();
    }

  private:
    Variant<Error, T> m_variant;
};

template <class T>
class Result<T &>
{
  public:
    inline Result()
    {
    }

    inline Result(const Result & _result) : m_variant(_result.m_variant)
    {
    }

    inline Result(Result && _result) : m_variant(std::move(_result.m_variant))
    {
    }

    inline Result(T & _result) : m_variant(&_result)
    {
    }

    inline Result(const Error & _error) : m_variant(_error)
    {
    }

    inline operator bool() const
    {
        return m_variant.isValid() && !m_variant.template is<Error>();
    }

    inline const Error & error() const
    {
        STICK_ASSERT(m_variant.template is<Error>());
        return m_variant.template get<Error>();
    }

    inline void setError(const Error & _error)
    {
        m_variant = _error;
    }

    inline void setError(Error && _error)
    {
        m_variant = std::move(_error);
    }

    inline const T & ensure() const
    {
        if (!m_variant.isValid() || m_variant.template is<Error>())
        {
            printf("Called ensure on an empty result.\n");
            if (m_variant.template is<Error>())
            {
                printf(
                    "The result holds the following error message: %s.\nThe generic error "
                    "description is: %s.\n",
                    m_variant.template get<Error>().message().cString(),
                    m_variant.template get<Error>().description().cString());
            }
            exit(EXIT_FAILURE);
        }
        return *m_variant.template get<T *>();
    }

    inline T & ensure()
    {
        return const_cast<T &>(const_cast<const Result &>(*this).ensure());
    }

    T & get()
    {
        return *m_variant.template get<T *>();
    }

    const T & get() const
    {
        return *m_variant.template get<T *>();
    }

  private:
    Variant<Error, T *> m_variant;
};

template <class T>
inline constexpr Result<typename std::decay<T>::type> makeResult(T && _value)
{
    return Result<typename std::decay<T>::type>(std::forward<T>(_value));
}

// STICK_RESULT_HOLDER(TextResultHolder, text);
typedef Result<stick::String> TextResult;
} // namespace stick

#endif // STICK_RESULT_HPP
