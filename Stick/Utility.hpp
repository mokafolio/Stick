#ifndef STICK_UTILITY_HPP
#define STICK_UTILITY_HPP

#include <Stick/Iterator.hpp>
#include <type_traits>
#include <utility>

// stuff that needs to sit outside of the stick namespace
template <class T>
static constexpr bool enableBitmaskOperators(T)
{
    return false;
}

template <typename T>
typename std::enable_if<enableBitmaskOperators(T()), T>::type operator|(T _a, T _b)
{
    typedef typename std::underlying_type<T>::type underlying;
    return static_cast<T>(static_cast<underlying>(_a) | static_cast<underlying>(_b));
}

template <typename T>
typename std::enable_if<enableBitmaskOperators(T()), T>::type operator&(T _a, T _b)
{
    typedef typename std::underlying_type<T>::type underlying;
    return static_cast<T>(static_cast<underlying>(_a) & static_cast<underlying>(_b));
}

namespace stick
{
inline uintptr_t alignOffset(const void * _ptr, int _alignment)
{
    return ((~reinterpret_cast<uintptr_t>(_ptr)) + 1) & (_alignment - 1);
}

template <typename T>
inline uintptr_t memoryAlignOffset(const void * _ptr)
{
    return alignOffset(_ptr, sizeof(T));
}

template <typename T>
inline T * memoryAlign(T * ptr)
{
    uintptr_t offset = memoryAlignOffset<uintptr_t>(ptr);
    char * aligned_ptr = reinterpret_cast<char *>(ptr) + offset;
    return reinterpret_cast<T *>(aligned_ptr);
}

template <class T>
bool hasFields(T _mask, T _fields)
{
    return (_mask & _fields) == _fields;
}

template <class T>
inline T min(const T & _a, const T & _b)
{
    return _a < _b ? _a : _b;
}

template <class T>
inline T max(const T & _a, const T & _b)
{
    return _a > _b ? _a : _b;
}

template <class F>
class ScopeExit
{
  public:
    ScopeExit(F && _func) : m_function(std::move(_func)), m_bCallInDestructor(true)
    {
    }

    ScopeExit(ScopeExit && _ex) :
        m_function(std::move(_ex.m_function)),
        m_bCallInDestructor(std::move(_ex.m_bCallInDestructor))
    {
    }

    ~ScopeExit()
    {
        if (m_bCallInDestructor)
            m_function();
    }

    void cancel()
    {
        m_bCallInDestructor = false;
    }

  private:
    ScopeExit(const ScopeExit &) = delete;
    void operator=(const ScopeExit &) = delete;
    ScopeExit & operator=(ScopeExit &&) = delete;

    F m_function;
    bool m_bCallInDestructor;
};

template <class F>
ScopeExit<typename std::remove_reference<F>::type> makeScopeExit(F && _f)
{
    return ScopeExit<typename std::remove_reference<F>::type>(std::forward<F>(_f));
}

template <class InputIter, class T>
InputIter find(InputIter _first, InputIter _last, const T & _value)
{
    for (; _first != _last; ++_first)
    {
        if (*_first == _value)
        {
            return _first;
        }
    }
    return _last;
}

template <class InputIter, class Pred>
InputIter findIf(InputIter _first, InputIter _last, Pred _p)
{
    for (; _first != _last; ++_first)
    {
        if (_p(*_first))
        {
            return _first;
        }
    }
    return _last;
}
} // namespace stick

#endif // STICK_UTILITY_HPP
