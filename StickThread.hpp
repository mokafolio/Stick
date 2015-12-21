#ifndef STICK_THREAD_HPP
#define STICK_THREAD_HPP

#include <Stick/StickError.hpp>

#ifdef STICK_PLATFORM_UNIX
#include <pthread.h>
#endif //STICK_PLATFORM_UNIX

namespace stick
{
    typedef Size ThreadID;

    class Thread
    {
    public:

#ifdef STICK_PLATFORM_UNIX
        typedef pthread_t NativeHandle;
#endif //STICK_PLATFORM_UNIX

        Thread();

        ~Thread();

        template<class F>
        inline Error run(F _func);

        Error join();

        NativeHandle nativeHandle();

        bool joinable() const;

        ThreadID id() const;

    private:

        NativeHandle m_handle;
    };
}

#endif //STICK_THREAD_HPP
