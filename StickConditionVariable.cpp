#include <Stick/StickConditionVariable.hpp>
#include <Stick/StickMutex.hpp>

namespace stick
{
    ConditionVariable::ConditionVariable() :
        m_bIsInitialized(false)
    {

    }

    ConditionVariable::~ConditionVariable()
    {

    }

    void ConditionVariable::notifyOne()
    {

    }

    void ConditionVariable::notifyAll()
    {

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
        if(!_lock.isLocked())
            return Error(ec::InvalidOperation, "The lock either does not have a mutex assigned or did not lock the mutex.", STICK_FILE, STICK_LINE);
        auto mtx = _lock.mutex()->nativeHandle();
        int res = pthread_cond_wait(&m_handle, &mtx);
        if (res != 0)
                return Error(ec::SystemErrorCode(res), "pthread_cond_wait failed.", STICK_FILE, STICK_LINE);
        return Error();
#endif //STICK_PLATFORM_UNIX
    }

    ConditionVariable::NativeHandle ConditionVariable::nativeHandle() const
    {
        return m_handle;
    }
}
