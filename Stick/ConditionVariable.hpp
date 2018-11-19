#ifndef STICK_CONDITIONVARIABLE_HPP
#define STICK_CONDITIONVARIABLE_HPP

#include <Stick/Error.hpp>
#include <Stick/ErrorCodes.hpp>
#include <Stick/ScopedLock.hpp>

#ifdef STICK_PLATFORM_UNIX
#include <pthread.h>
#endif // STICK_PLATFORM_UNIX

namespace stick
{
class Mutex;

class STICK_API ConditionVariable
{
  public:
#ifdef STICK_PLATFORM_UNIX
    typedef pthread_cond_t Native;
    typedef Native * NativeHandle;
#endif // STICK_PLATFORM_UNIX

    typedef ScopedLock<Mutex> LockType;

    ConditionVariable();

    ConditionVariable(const ConditionVariable & _other) = delete;
    ConditionVariable(ConditionVariable && _other) = delete;
    ConditionVariable & operator=(const ConditionVariable & _other) = delete;
    ConditionVariable & operator=(ConditionVariable && _other) = delete;

    ~ConditionVariable();

    Error notifyOne();

    Error notifyAll();

    Error wait(LockType & _lock);

    template <class F>
    Error wait(LockType & _lock, F && _predicate);

    NativeHandle nativeHandle();

  private:
    Native m_handle;
    bool m_bIsInitialized;
};
} // namespace stick

#endif // STICK_CONDITIONVARIABLE_HPP
