#ifndef STICK_THREAD_HPP
#define STICK_THREAD_HPP

#include <Stick/StickError.hpp>
#include <Stick/StickErrorCodes.hpp>

#ifdef STICK_PLATFORM_UNIX
#include <pthread.h>
#include <new> //for std::nothrow
#endif //STICK_PLATFORM_UNIX

#include <iostream>
#include <atomic>

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

        ThreadID threadID() const;

        static ThreadID currentThreadID();


    private:

        NativeHandle m_handle;
        std::atomic<ThreadID> m_threadID;
    };

#ifdef STICK_PLATFORM_UNIX
    namespace detail
    {
        struct PThreadDataBase
        {
            PThreadDataBase(Thread * _t) :
            t(_t)
            {

            }

            virtual ~PThreadDataBase() {}

            virtual void call() = 0;

            Thread * t;
        };

        template<class F>
        struct PThreadData : public PThreadDataBase
        {
            PThreadData(Thread * _t, F _f) :
            PThreadDataBase(_t),
            func(_f)
            {
            }

            void call() override
            {
                func();
            }

            F func;
        };

        inline Size _pthreadID(pthread_t _handle)
        {
            static_assert(sizeof(Size) >= sizeof(pthread_t), "pthread_t is bigger than Size.");
            Size ret = 0;
            for(UInt32 i = 0; i < sizeof(pthread_t); ++i)
            {
                char * from = reinterpret_cast<char*>(&_handle) + i;
                char * to = reinterpret_cast<char*>(&ret) + i;
                *to = *from;
            }
            return ret;
        }

        inline void * _pthreadFunc(void * _data)
        {
            PThreadDataBase * data = reinterpret_cast<PThreadDataBase*>(_data);
            data->call();
            delete data;
            return NULL;
        }
    }
#endif //STICK_PLATFORM_UNIX

    template<class F>
    inline Error Thread::run(F _func)
    {
#ifdef STICK_PLATFORM_UNIX
        detail::PThreadDataBase * data = new (std::nothrow) detail::PThreadData<F>(this, _func);
        int res = pthread_create(&m_handle, NULL, &detail::_pthreadFunc, data);
        if(res != 0)
            return Error(ec::SystemErrorCode(res), "Could not create pthread", STICK_FILE, STICK_LINE);
#endif //STICK_PLATFORM_UNIX
        return Error();
    }
}

#endif //STICK_THREAD_HPP
