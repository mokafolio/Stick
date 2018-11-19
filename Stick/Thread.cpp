#include <Stick/ErrorCodes.hpp>
#include <Stick/Thread.hpp>
#include <time.h>

#include <iostream>

namespace stick
{
void * Thread::_pthreadFunc(void * _data)
{
    detail::PThreadDataBase * data = reinterpret_cast<detail::PThreadDataBase *>(_data);
    data->call();
    {
        ScopedLock<Mutex> lock(data->t->m_mutex);
        data->t->m_bIsJoinable = false;
    }
    delete data;
    return NULL;
}

Thread::Thread() : m_bIsJoinable(false), m_threadID(0)
{
}

Thread::Thread(Thread && _other) :
    m_handle(std::move(_other.m_handle)),
    m_bIsJoinable(std::move(_other.m_bIsJoinable)),
    m_threadID(std::move(_other.m_threadID))
{
    ScopedLock<Mutex> lock(_other.m_mutex);
    _other.m_bIsJoinable = false;
    _other.m_threadID = 0;
}

Thread::~Thread()
{
    STICK_ASSERT(!isJoinable());
}

Thread & Thread::operator=(Thread && _other)
{
    STICK_ASSERT(!isJoinable());
    m_handle = std::move(_other.m_handle);
    m_threadID = std::move(_other.m_threadID);
    m_bIsJoinable = std::move(_other.m_bIsJoinable);

    ScopedLock<Mutex> lock(_other.m_mutex);
    _other.m_bIsJoinable = false;
    _other.m_threadID = 0;

    return *this;
}

Error Thread::join()
{
#ifdef STICK_PLATFORM_UNIX
    {
        ScopedLock<Mutex> lock(m_mutex);
        if (m_threadID == 0) // thread is not initialized
            return Error();
    }
    int res = pthread_join(m_handle, NULL);
    m_threadID = 0;
    if (res != 0)
        return Error(ec::SystemErrorCode(res), "Could not join pthread", STICK_FILE, STICK_LINE);
#endif // STICK_PLATFORM_UNIX

    ScopedLock<Mutex> lock(m_mutex);
    m_bIsJoinable = false;
    return Error();
}

Thread::NativeHandle Thread::nativeHandle()
{
    return &m_handle;
}

bool Thread::isJoinable() const
{
    ScopedLock<Mutex> lock(m_mutex);
    return m_bIsJoinable;
}

ThreadID Thread::threadID() const
{
    ScopedLock<Mutex> lock(m_mutex);
    return m_threadID;
}

void Thread::sleepFor(const Duration & _dur)
{
    auto dur = _dur.nanoseconds();

    if (dur <= 0)
        return;

    timespec dr;
    dr.tv_sec = static_cast<Int64>(_dur.seconds());
    dr.tv_nsec = dur - dr.tv_sec * 1000000000;
    while (nanosleep(&dr, &dr))
        ;
}

ThreadID Thread::currentThreadID()
{
#ifdef STICK_PLATFORM_UNIX
    pthread_t pt = pthread_self();
    return detail::_pthreadID(pt);
#endif // STICK_PLATFORM_UNIX
}
} // namespace stick
