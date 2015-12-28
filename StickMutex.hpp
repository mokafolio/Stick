#ifndef STICK_MUTEX_HPP
#define STICK_MUTEX_HPP

#include <Stick/StickError.hpp>
#include <Stick/StickErrorCodes.hpp>
#include <Stick/StickUtility.hpp>

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

        bool tryLock();

        void unlock();

        NativeHandle nativeHandle() const;


    private:

        NativeHandle m_handle;
        bool m_bIsInitialized;
    };

    namespace detail
    {
        struct ScopedLock
        {
            ScopedLock(Mutex & _mutex) :
                mutex(_mutex)
            {
                auto err = mutex.lock();
                STICK_ASSERT(err.code() == 0);
            }

            ~ScopedLock()
            {
                mutex.unlock();
            }

            Mutex mutex;
        };
    }

    inline detail::ScopedLock lockScope(Mutex & _mutex)
    {
        return detail::ScopedLock(_mutex);
    }
}

#endif //STICK_MUTEX_HPP
