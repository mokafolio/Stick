#ifndef STICK_STICKRESULT_HPP
#define STICK_STICKRESULT_HPP

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
        _holderName(_data) \
    { \
 \
    } \
 \
    _name(T && _data) : \
        _holderName(move(_data)) \
    { \
 \
    } \
 \
    T _holderName; \
} \

namespace stick
{
    STICK_RESULT_HOLDER(DefaultResultHolder, data);

    template<class T, template<class> class ResultHolder = DefaultResultHolder>
    struct Result : public ResultHolder<T>
    {
        typedef ResultHolder<T> ResultHolderType;

        inline Result()
        {

        }

        inline Result(const T & _result, const Error & _error) :
            ResultHolderType(_result),
            error(_error)
        {

        }

        inline Result(T && _result, const Error & _error) :
            ResultHolderType(move(_result)),
            error(_error)
        {

        }

        inline operator bool() const
        {
            return !static_cast<bool>(error);
        }

        Error error;
    };
}

#endif //STICK_STICKRESULT_HPP
