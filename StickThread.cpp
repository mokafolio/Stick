#include <Stick/StickThread.hpp>
#include <Stick/StickErrorCodes.hpp>

namespace stick
{
    Thread::Thread()
    {

    }

    Thread::~Thread()
    {
        join();
    }

    Error Thread::join()
    {
#ifdef STICK_PLATFORM_UNIX
        int res = pthread_join(m_handle, NULL);
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

    }

    ThreadID Thread::id() const
    {

    }
}
