#include <Stick/Mutex.hpp>

namespace stick
{
Mutex::Mutex() : m_bIsInitialized(false)
{
}

Mutex::~Mutex()
{
    if (m_bIsInitialized)
    {
        STICK_ASSERT(pthread_mutex_destroy(&m_handle) == 0);
    }
}

Error Mutex::lock()
{
#ifdef STICK_PLATFORM_UNIX
    if (!m_bIsInitialized)
    {
        int res = pthread_mutex_init(&m_handle, NULL);
        if (res != 0)
            return Error(ec::SystemErrorCode(res),
                         "Could not initialize pthread_mutex",
                         STICK_FILE,
                         STICK_LINE);
        m_bIsInitialized = true;
    }
    int res = pthread_mutex_lock(&m_handle);
    if (res != 0)
        return Error(
            ec::SystemErrorCode(res), "Could not lock pthread_mutex", STICK_FILE, STICK_LINE);
#endif // STICK_PLATFORM_UNIX
    return Error();
}

bool Mutex::tryLock()
{
#ifdef STICK_PLATFORM_UNIX
    return pthread_mutex_trylock(&m_handle) == 0 ? true : false;
#endif // STICK_PLATFORM_UNIX
}

void Mutex::unlock()
{
#ifdef STICK_PLATFORM_UNIX
    // we ignore errors from unlock
    STICK_ASSERT(m_bIsInitialized);
    pthread_mutex_unlock(&m_handle);
#endif // STICK_PLATFORM_UNIX
}

Mutex::NativeHandle Mutex::nativeHandle()
{
    return &m_handle;
}
} // namespace stick
