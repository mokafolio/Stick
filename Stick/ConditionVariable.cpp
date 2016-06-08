#include <Stick/ConditionVariable.hpp>
#include <Stick/Mutex.hpp>

namespace stick
{
    ConditionVariable::ConditionVariable() :
        m_bIsInitialized(false)
    {

    }

    ConditionVariable::~ConditionVariable()
    {
        if (m_bIsInitialized)
        {
            STICK_ASSERT(pthread_cond_destroy(&m_handle) == 0);
        }
    }

    Error ConditionVariable::notifyOne()
    {
        if (!m_bIsInitialized) return Error();
#ifdef STICK_PLATFORM_UNIX
        int res = pthread_cond_signal(&m_handle);
        if (res != 0)
            return Error(ec::SystemErrorCode(res), "pthread_cond_signal failed.", STICK_FILE, STICK_LINE);
        return Error();
#endif //STICK_PLATFORM_UNIX
    }

    Error ConditionVariable::notifyAll()
    {
        if (!m_bIsInitialized) return Error();
#ifdef STICK_PLATFORM_UNIX
        int res = pthread_cond_broadcast(&m_handle);
        if (res != 0)
            return Error(ec::SystemErrorCode(res), "pthread_cond_broadcast failed.", STICK_FILE, STICK_LINE);
        return Error();
#endif //STICK_PLATFORM_UNIX
    }

    Error ConditionVariable::wait(LockType & _lock)
    {
#ifdef STICK_PLATFORM_UNIX
        if (!m_bIsInitialized)
        {
            int res = pthread_cond_init(&m_handle, NULL);
            if (res != 0)
                return Error(ec::SystemErrorCode(res), "Could not initialize pthread condition variable", STICK_FILE, STICK_LINE);
            m_bIsInitialized = true;
        }
        if (!_lock.isLocked())
            return Error(ec::InvalidOperation, "The lock either does not have a mutex assigned or did not lock the mutex.", STICK_FILE, STICK_LINE);
        int res = pthread_cond_wait(&m_handle, _lock.mutex()->nativeHandle());
        if (res != 0)
            return Error(ec::SystemErrorCode(res), "pthread_cond_wait failed.", STICK_FILE, STICK_LINE);
        return Error();
#endif //STICK_PLATFORM_UNIX
    }

    ConditionVariable::NativeHandle ConditionVariable::nativeHandle()
    {
        return &m_handle;
    }
}
