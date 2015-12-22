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
        ThreadID m_threadID;
    };

#ifdef STICK_PLATFORM_UNIX
    namespace detail
    {
        struct PThreadDataBase
        {
            virtual ~PThreadDataBase() {}

            virtual void call() = 0;
        };

        template<class F>
        struct PThreadData
        {
            PThreadData(F _f) :
            func(_f)
            {
            }

            void call()
            {
                func();
            }

            F func;
        };

        inline void * _pthreadFunc(void * _data)
        {
            PThreadDataBase * data = reinterpret_cast<PThreadDataBase*>(_data);
            data->call();
        }
    }
#endif //STICK_PLATFORM_UNIX

    template<class F>
    inline Error Thread::run(F _func)
    {
#ifdef STICK_PLATFORM_UNIX
        detail::PThreadDataBase * data = new detail::PThreadData<F>(_func);
        int res = pthread_create(&m_handle, NULL, &detail::_pthreadFunc, data);
        if(res != 0)
            return Error(ec::SystemErrorCode(res), "Could not create pthread", STICK_FILE, STICK_LINE);
#endif //STICK_PLATFORM_UNIX
        return Error();
    }
}

#endif //STICK_THREAD_HPP
