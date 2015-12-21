#include <Stick/StickThread.hpp>

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
        pthread_join(m_handle, NULL);
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
