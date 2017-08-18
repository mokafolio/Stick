#ifndef STICK_MUTEX_HPP
#define STICK_MUTEX_HPP

#include <Stick/Error.hpp>
#include <Stick/ErrorCodes.hpp>
#include <Stick/Utility.hpp>

#ifdef STICK_PLATFORM_UNIX
#include <pthread.h>
#endif //STICK_PLATFORM_UNIX

namespace stick
{
    class STICK_API NoMutex
    {
    public:

        inline NoMutex()
        {
        }

        inline ~NoMutex()
        {
        }

        inline Error lock()
        {
            return Error();
        }

        inline bool tryLock()
        {
            return true;
        }

        inline void unlock()
        {
        }
    };

    class STICK_API Mutex
    {
    public:

#ifdef STICK_PLATFORM_UNIX
        typedef pthread_mutex_t Native;
        typedef Native * NativeHandle;
#endif //STICK_PLATFORM_UNIX

        Mutex();

        Mutex(const Mutex & _other) = delete;
        Mutex(Mutex && _other) = delete;
        Mutex & operator = (const Mutex & _other) = delete;
        Mutex & operator = (Mutex && _other) = delete;

        ~Mutex();

        Error lock();

        bool tryLock();

        void unlock();

        NativeHandle nativeHandle();


    private:

        Native m_handle;
        bool m_bIsInitialized;
    };
}

#endif //STICK_MUTEX_HPP
