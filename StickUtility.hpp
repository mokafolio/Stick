#ifndef STICK_UTILITY_HPP
#define STICK_UTILITY_HPP

#include <Stick/StickAllocator.hpp>
#include <Stick/StickIterator.hpp>

namespace stick
{
    template<class T>
    struct RemoveReference
    {
        typedef T Type;
    };

    template<class T>
    struct RemoveReference<T &>
    {
        typedef T Type;
    };

    template<class T>
    struct RemoveReference < T && >
    {
        typedef T Type;
    };

    template<class T>
    inline typename RemoveReference<T>::Type && move(T && _arg)
    {
        return static_cast < typename RemoveReference<T>::Type && > (_arg);
    }

    template<class T>
    inline T min(const T & _a, const T & _b)
    {
        return _a < _b ? _a : _b;
    }

    template<class T>
    inline T max(const T & _a, const T & _b)
    {
        return _a > _b ? _a : _b;
    }
}

#endif //STICK_UTILITY_HPP
