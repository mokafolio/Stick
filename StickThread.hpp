#ifndef STICK_THREAD_HPP
#define STICK_THREAD_HPP

#include <Stick/StickPlatform.hpp>

#ifdef STICK_PLATFORM_UNIX
#include <pthread.h>
#endif //STICK_PLATFORM_UNIX

namespace stick
{
    class Thread
    {
    public:

        Thread(){}

        ~Thread();

        

    private:

#ifdef STICK_PLATFORM_UNIX
        pthread_t m_pthread;
#endif //STICK_PLATFORM_UNIX
    };
}

#endif //STICK_THREAD_HPP
