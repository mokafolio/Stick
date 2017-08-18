#ifndef STICK_THREAD_HPP
#define STICK_THREAD_HPP

#include <Stick/Mutex.hpp>
#include <Stick/ScopedLock.hpp>
#include <Stick/TimePoint.hpp>

#ifdef STICK_PLATFORM_UNIX
#include <pthread.h>
#include <new> //for std::nothrow
#endif //STICK_PLATFORM_UNIX

namespace stick
{
    typedef Size ThreadID;

    class STICK_API Thread
    {
    public:

#ifdef STICK_PLATFORM_UNIX
        typedef pthread_t Native;
        typedef pthread_t * NativeHandle;
#endif //STICK_PLATFORM_UNIX

        Thread();

        Thread(Thread && _other);

        Thread(const Thread & _other) = delete;
        Thread & operator = (const Thread & _other) = delete;

        Thread & operator = (Thread && _other);

        ~Thread();

        template<class F>
        inline Error run(F && _func);

        Error join();

        NativeHandle nativeHandle();

        bool isJoinable() const;

        ThreadID threadID() const;

        static ThreadID currentThreadID();

        static void sleepFor(const Duration & _dur);

        template<class C, class R>
        static void sleepUntil(const TimePointT<C, R> & _tp);

    private:

#ifdef STICK_PLATFORM_UNIX
        static void * _pthreadFunc(void * _data);
#endif //STICK_PLATFORM_UNIX

        Native m_handle;
        mutable Mutex m_mutex;
        bool m_bIsJoinable;
        ThreadID m_threadID;
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
            PThreadData(Thread * _t, F && _f) :
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
            for (UInt32 i = 0; i < sizeof(pthread_t); ++i)
            {
                char * from = reinterpret_cast<char *>(&_handle) + i;
                char * to = reinterpret_cast<char *>(&ret) + i;
                *to = *from;
            }
            return ret;
        }
    }
#endif //STICK_PLATFORM_UNIX

    template<class F>
    inline Error Thread::run(F && _func)
    {
#ifdef STICK_PLATFORM_UNIX
        ScopedLock<Mutex> lock(m_mutex);
        m_bIsJoinable = true;
        detail::PThreadDataBase * data = new (std::nothrow) detail::PThreadData<F>(this, std::forward<F>(_func));
        int res = pthread_create(&m_handle, NULL, &Thread::_pthreadFunc, data);
        if (res != 0)
        {
            m_bIsJoinable = false;
            return Error(ec::SystemErrorCode(res), "Could not create pthread", STICK_FILE, STICK_LINE);
        }
        m_threadID = detail::_pthreadID(m_handle);
#endif //STICK_PLATFORM_UNIX
        return Error();
    }

    template<class C, class R>
    inline void Thread::sleepUntil(const TimePointT<C, R> & _tp)
    {
        Duration dur = _tp - C::now();
        if(dur.nanoseconds() > 0)
            sleepFor(dur);
    }
}

#endif //STICK_THREAD_HPP
