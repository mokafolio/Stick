#ifndef STICK_STICKRESULT_HPP
#define STICK_STICKRESULT_HPP

namespace stick
{
    template<class T>
    struct DefaultResultHolder
    {
        DefaultResultHolder()
        {

        }

        DefaultResultHolder(const T & _data) :
        data(_data)
        {

        }

        T data;
    };

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

        inline operator bool() const
        {
            return !static_cast<bool>(error);
        }

        Error error;
    };
}

#endif //STICK_STICKRESULT_HPP
