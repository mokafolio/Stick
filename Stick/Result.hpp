#ifndef STICK_STICKRESULT_HPP
#define STICK_STICKRESULT_HPP

#include <Stick/Maybe.hpp>

#define STICK_RESULT_HOLDER(_name, _holderName) \
template<class T> \
struct _name \
{ \
    _name() \
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
        m_value(move(_data)) \
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
    stick::Maybe<T> m_value; \
} \

namespace stick
{
    STICK_RESULT_HOLDER(DefaultResultHolder, data);

    template<class T, template<class> class ResultHolder = DefaultResultHolder>
    class Result : public ResultHolder<T>
    {
    public:

        typedef ResultHolder<T> ResultHolderType;

        inline Result()
        {

        }

        inline Result(const T & _result) :
            ResultHolderType(_result)
        {

        }

        inline Result(T && _result) :
            ResultHolderType(move(_result))
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
            m_error = move(_error);
        }

        inline const T & ensure()
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

    private:

        Error m_error;
    };
}

#endif //STICK_STICKRESULT_HPP
