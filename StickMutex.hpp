#ifndef STICK_MUTEX_HPP
#define STICK_MUTEX_HPP

#include <Stick/StickError.hpp>
#include <Stick/StickErrorCodes.hpp>

#ifdef STICK_PLATFORM_UNIX
#include <pthread.h>
#endif //STICK_PLATFORM_UNIX

namespace stick
{
    class Mutex
    {
    public:

#ifdef STICK_PLATFORM_UNIX
        typedef pthread_mutex_t NativeHandle;
#endif //STICK_PLATFORM_UNIX

        Mutex();

        ~Mutex();

        Error lock();

        void unlock();

        NativeHandle nativeHandle() const;


    private:

        NativeHandle m_handle;

        bool m_bIsInitialized;
    };
}

#endif //STICK_MUTEX_HPP
