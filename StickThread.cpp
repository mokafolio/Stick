#include <Stick/StickThread.hpp>
#include <Stick/StickErrorCodes.hpp>

namespace stick
{
    Thread::Thread() :
    m_threadID(0)
    {

    }

    Thread::~Thread()
    {
        /*if(joinable())
            join();*/
    }

    Error Thread::join()
    {
#ifdef STICK_PLATFORM_UNIX
        int res = pthread_join(m_handle, NULL);
        m_threadID = 0;
        if(res != 0)
            return Error(ec::SystemErrorCode(res), "Could not join pthread", STICK_FILE, STICK_LINE);
#endif //STICK_PLATFORM_UNIX

        return Error();
    }

    Thread::NativeHandle Thread::nativeHandle()
    {
        return m_handle;
    }

    bool Thread::joinable() const
    {
        return m_threadID != 0;
    }

    ThreadID Thread::threadID() const
    {
        return m_threadID;
    }

    ThreadID Thread::currentThreadID()
    {
#ifdef STICK_PLATFORM_UNIX
        pthread_t pt = pthread_self();
        return detail::_pthreadID(pt);
#endif //STICK_PLATFORM_UNIX
    }
}
